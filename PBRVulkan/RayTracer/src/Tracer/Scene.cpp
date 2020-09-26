#include "Scene.h"

#include "../Vulkan/Device.h"
#include "../Vulkan/CommandPool.h"
#include "../Vulkan/Buffer.h"

#include "../Geometry/Vertex.h"

#include "../Assets/TextureImage.h"

namespace Tracer
{
	Scene::Scene(const Vulkan::Device& device, const Vulkan::CommandPool& commandPool)
		: device(device), commandPool(commandPool)
	{
		CreateBuffers();

		textureImage.reset(new Assets::TextureImage(device, commandPool, "../Assets/Textures/statue.jpg"));
	}

	Scene::~Scene() {}

	void Scene::CreateBuffers()
	{
		const std::vector<Uniforms::Vertex> vertices = {
			{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

			{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		// =============== VERTEX BUFFER ===============

		auto size = sizeof(vertices[0]) * vertices.size();

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
}
