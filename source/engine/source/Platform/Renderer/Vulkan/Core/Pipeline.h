#pragma once

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


namespace Helios::Vulkan {


	class Devices;
	class Swapchain;


	class Pipeline
	{
	public:
		Pipeline();
		~Pipeline() = default;

		void Create();
		void Destroy();

	public:

	private:
		vk::PipelineLayout CreateLayout();
		vk::RenderPass CreateRenderPass();

	private:
		// Class/Object pointers
		Ref<Devices> m_Devices;
		Ref<Swapchain> m_Swapchain;

		// Vulkan objects
		vk::PipelineLayout m_vkLayout;
		vk::RenderPass m_vkRenderPass;
		vk::Pipeline m_vkPipeline;
	};


} // namespace Helios::Vulkan
