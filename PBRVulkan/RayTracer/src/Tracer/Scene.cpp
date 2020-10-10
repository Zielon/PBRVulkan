#include "Scene.h"

#include <iostream>


#include "TextureImage.h"
#include "Camera.h"

#include "../Vulkan/Device.h"
#include "../Vulkan/CommandPool.h"
#include "../Vulkan/Buffer.h"

#include "../Geometry/Vertex.h"

#include "../Assets/Material.h"
#include "../Assets/Light.h"
#include "../Assets/Texture.h"
#include "../Assets/Mesh.h"

#include "../Loader/Loader.h"
#include "../Loader/RenderOptions.h"

namespace Tracer
{
	Scene::Scene(
		const std::string& config,
		const Vulkan::Device& device,
		const Vulkan::CommandPool& commandPool)
		: config(config), device(device), commandPool(commandPool)
	{
		Load();
		Process();
		CreateBuffers();

		std::cout << "[INFO] Scene has been loaded!" << std::endl;
	}

	void Scene::Load()
	{
		Loader::RenderOptions options;
		LoadSceneFromFile(config, *this, options);

		// Release loaded pixles from Texture objects
		textures.clear();
	}

	void Scene::Process()
	{
		const auto identity = glm::mat4(1.f);

		/*
		 * Translation of position does not affect normals.
		 * Rotation is applied to normals just like it is to position.
		 * Uniform scaling of position does not affect the direction of normals
		 * Non-uniform scaling of position does affect the direction of normals!
		 */
#pragma omp parallel for
		for (auto& meshInstance : meshInstances)
		{
			glm::mat4 modelMatrix = meshInstance.modelTransform;

			if (modelMatrix == identity) continue;

			for (auto& vertex : meshes[meshInstance.meshId]->GetVertices())
			{
				vertex.position = modelMatrix * glm::vec4(vertex.position.xyz(), 1.f);
				vertex.materialId = meshInstance.materialId;
			}
		}
	}

	void Scene::CreateBuffers()
	{
		std::vector<uint32_t> indices;
		std::vector<Geometry::Vertex> vertices;

		for (const auto& mesh : meshes)
		{
			vertices.insert(vertices.end(), mesh->GetVertices().begin(), mesh->GetVertices().end());
			indices.insert(indices.end(), mesh->GetIndecies().begin(), mesh->GetIndecies().end());
		}

		verticesSize = vertices.size();
		indeciesSize = indices.size();

		// =============== VERTEX BUFFER ===============

		auto size = sizeof(meshes[0]->GetVertices()[0]) * verticesSize;

		std::cout << "[INFO] Vertex buffer size = " << static_cast<double>(size) / 1000000.0 << " MB" << std::endl;

		std::unique_ptr<Vulkan::Buffer> buffer_staging(
			new Vulkan::Buffer(
				device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		buffer_staging->Fill(vertices.data());

		vertexBuffer.reset(
			new Vulkan::Buffer(
				device, size,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		vertexBuffer->Copy(commandPool, *buffer_staging);

		// =============== INDEX BUFFER ===============

		size = sizeof(indices[0]) * indices.size();

		buffer_staging.reset(
			new Vulkan::Buffer(
				device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		buffer_staging->Fill(indices.data());

		indexBuffer.reset(
			new Vulkan::Buffer(
				device, size,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		indexBuffer->Copy(commandPool, *buffer_staging);

		// =============== MATERIAL BUFFER ===============

		size = sizeof(materials[0]) * materials.size();

		buffer_staging.reset(
			new Vulkan::Buffer(
				device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		buffer_staging->Fill(materials.data());

		materialBuffer.reset(
			new Vulkan::Buffer(
				device, size,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		materialBuffer->Copy(commandPool, *buffer_staging);
	}

	void Scene::AddCamera(glm::vec3 pos, glm::vec3 lookAt, float fov)
	{
		camera.reset(new Camera(pos, lookAt, fov));
	}

	void Scene::AddHDR(const std::string& path) { }

	int Scene::AddMeshInstance(Assets::MeshInstance meshInstance)
	{
		int id = meshInstances.size();
		meshInstances.push_back(meshInstance);
		return id;
	}

	int Scene::AddMesh(const std::string& path)
	{
		int id = -1;

		if (meshMap.find(path) != meshMap.end())
		{
			id = meshMap[path];
		}
		else
		{
			const auto file = "../Assets/Scenes/" + path;
			std::cout << "[MESH] " + file + " has been added!" << std::endl;
			id = meshes.size();
			meshes.emplace_back(new Assets::Mesh(file));
			meshMap[path] = id;
		}

		return id;
	}

	int Scene::AddTexture(const std::string& path)
	{
		if (textureMap.find(path) != textureMap.end())
		{
			return meshMap[path];
		}

		int id = textures.size();
		const auto file = "../Assets/Scenes/" + path;
		std::cout << "[TEXTURE] " + file + " has been added!" << std::endl;
		textures.emplace_back(new Assets::Texture(file));
		textureImages.emplace_back(new TextureImage(device, commandPool, *textures[id++]));
		textureMap[path] = id;
		return id;
	}

	int Scene::AddMaterial(Assets::Material material)
	{
		int id = materials.size();
		materials.push_back(material);
		return id;
	}

	int Scene::AddLight(Assets::Light light)
	{
		int id = lights.size();
		lights.push_back(light);
		return id;
	}

	Scene::~Scene() { }
}
