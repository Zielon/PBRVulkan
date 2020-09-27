#include "Scene.h"

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
		CreateBuffers();
	}

	void Scene::Load()
	{
		Loader::RenderOptions options;
		LoadSceneFromFile(config, *this, options);

		textureImages.emplace_back(new TextureImage(device, commandPool, *textures[0]));

		// Release loaded pixles from Texture objects
		textures.clear();
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
			id = meshes.size();
			meshes.emplace_back(new Assets::Mesh(path));
			meshMap[path] = id;
		}

		verticesSize += meshes[id]->GetVerticesSize();
		indeciesSize += meshes[id]->GetIndeciesSize();
		return id;
	}

	int Scene::AddTexture(const std::string& path)
	{
		if (textureMap.find(path) != textureMap.end())
		{
			return meshMap[path];
		}

		int id = textures.size();
		textures.emplace_back(new Assets::Texture(path));
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

	void Scene::CreateBuffers()
	{
		std::vector<uint32_t> indices;
		std::vector<Geometry::Vertex> vertices;

		for (const auto& mesh : meshes)
		{
			vertices.insert(vertices.end(), mesh->GetVertices().begin(), mesh->GetVertices().end());
			indices.insert(indices.end(), mesh->GetIndecies().begin(), mesh->GetIndecies().end());
		}

		// =============== VERTEX BUFFER ===============

		auto size = sizeof(meshes[0]->GetVertices()[0]) * verticesSize;

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
	}

	Scene::~Scene() { }
}
