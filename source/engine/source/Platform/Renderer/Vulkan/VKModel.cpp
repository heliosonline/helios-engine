#include "pch.h"
#include "VKModel.h"

#include "HeliosEngine/Renderer/Renderer.h"
#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


namespace Helios {


	VKModel::VKModel()
//	VKModel::VKModel(const std::vector<Vertex>& vertices)
	{
		LOG_RENDER_DEBUG("VKModel::VKModel()");

		std::vector<Vertex> vertices
		{
			{{ -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
			{{ 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{{ 0.0f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		CreateVertexBuffers(vertices);
	}


	VKModel::~VKModel()
	{
		LOG_RENDER_DEBUG("VKModel::~VKModel()");

		Scope<Vulkan::Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		device->GetLogicalDevice().destroyBuffer(m_vertexBuffer);
		device->GetLogicalDevice().freeMemory(m_vertexBufferMemory);
	}


	void VKModel::Draw()
	{
	}


	void VKModel::vkBind(vk::CommandBuffer &commandBuffer)
	{
		vk::Buffer buffers[] = { m_vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer.bindVertexBuffers(0, 1, buffers, offsets);
	}


	void VKModel::vkDraw(vk::CommandBuffer &commandBuffer)
	{
		commandBuffer.draw(m_vertexCount, 1, 0, 0);
	}


	void VKModel::CreateVertexBuffers(const std::vector<Vertex> &vertices)
	{
		Scope<Vulkan::Device>& device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		m_vertexCount = static_cast<uint32_t>(vertices.size());
		LOG_RENDER_ASSERT(m_vertexCount >= 3, "Vertex count must be at least 3!");

		vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
		device->CreateBuffer(
			bufferSize,
			vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			m_vertexBuffer,
			m_vertexBufferMemory);

		void* data = device->GetLogicalDevice().mapMemory(m_vertexBufferMemory, 0, bufferSize);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		device->GetLogicalDevice().unmapMemory(m_vertexBufferMemory);
	}


	std::vector<vk::VertexInputBindingDescription> VKModel::Vertex::GetBindingDescriptions()
	{
		std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);

		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

		return bindingDescriptions;
	}


	std::vector<vk::VertexInputAttributeDescription> VKModel::Vertex::GetAttributeDescriptions()
	{
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);

		// Vertex position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Vertex color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}


} // namespace Helios
