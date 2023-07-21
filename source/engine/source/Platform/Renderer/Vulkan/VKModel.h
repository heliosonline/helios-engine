#pragma once

#include "HeliosEngine/Renderer/Model.h"

#include <vulkan/vulkan.hpp>

//#include "Platform/Renderer/Vulkan/Core/Instance.h"
//#include "Platform/Renderer/Vulkan/Core/Device.h"
//#include "Platform/Renderer/Vulkan/Core/Swapchain.h"

//#include "Platform/Renderer/Vulkan/Core/Pipeline.h"


namespace Helios {


	class VKModel : public Model
	{
	public:

//		VKModel(const std::vector<Vertex>& vertices);
		VKModel();
		~VKModel();

		void Draw();

	// Methods for internal usage in the Helios::Vulkan namespace
	public:

		struct Vertex
		{
			glm::vec2 position;
			glm::vec4 color;

			static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();
		};

		void vkBind(vk::CommandBuffer &commandBuffer);
		void vkDraw(vk::CommandBuffer &commandBuffer);

	// Vertex data
	private:
		void CreateVertexBuffers(const std::vector<Vertex> &vertices);

		vk::Buffer m_vertexBuffer;
		vk::DeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;
	};


} // namespace Helios
