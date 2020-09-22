#include "Scene.h"

#include "../Vulkan/Device.h"
#include "../Vulkan/CommandBuffers.h"

namespace Tracer
{
	Scene::Scene(const Vulkan::Device& device, const Vulkan::CommandBuffers& commandBuffers)
		: device(device), commandBuffers(commandBuffers)
	{
		CreateBuffers();
	}

	Scene::~Scene() {}

	void Scene::CreateBuffers()
	{
		const std::vector<Geometry::Vertex> vertices = {
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -0.5f, 0.5f } },
			{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -0.5f } },
			{ { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } },
			{ { -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } }
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		// =============== VERTEX BUFFER ===============
		
		auto size = sizeof(vertices[0]) * vertices.size();

		std::unique_ptr<Vulkan::Buffer<Geometry::Vertex>> buffer_staging(
			new Vulkan::Buffer<Geometry::Vertex>(
				device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		buffer_staging->Fill(vertices.data());

		vertexBuffer.reset(
			new Vulkan::Buffer<Geometry::Vertex>(
				device, size,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		vertexBuffer->Copy(commandBuffers, *buffer_staging);

		// =============== INDEX BUFFER ===============
		
		size = sizeof(indices[0]) * indices.size();

		buffer_staging.reset(
			new Vulkan::Buffer<Geometry::Vertex>(
				device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		buffer_staging->Fill(indices.data());

		indexBuffer.reset(
			new Vulkan::Buffer<Geometry::Vertex>(
				device, size,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		indexBuffer->Copy(commandBuffers, *buffer_staging);
	}
}
