#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"

#include "Platform/Renderer/Vulkan/Core/Instance.h"
#include "Platform/Renderer/Vulkan/Core/Device.h"
#include "Platform/Renderer/Vulkan/Core/Swapchain.h"

#include "Platform/Renderer/Vulkan/Core/Pipeline.h"

#include "Platform/Renderer/Vulkan/VKModel.h"


namespace Helios {


	class VKRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void Shutdown() override;

		void Render();
		void OnWindowResize(uint32_t width, uint32_t height);
		void OnFramebufferResize(uint32_t width, uint32_t height);

	// Methods for internal usage in the Helios::Vulkan namespace
	public:
		Scope<Vulkan::Instance>& GetInstance() { return m_Instance; }
		Scope<Vulkan::Device>& GetDevice() { return m_Device; }
		Ref<Vulkan::Swapchain>& GetSwapchain() { return m_Swapchain; }

	// Objects from the Helios::Vulkan namespace
	private:
		Ref<VKModel> m_model;

		Scope<Vulkan::Instance> m_Instance;
		Scope<Vulkan::Device> m_Device;
		Ref<Vulkan::Swapchain> m_Swapchain;

		Scope<Vulkan::Pipeline> m_Pipeline;
		vk::PipelineLayout m_vkPipelineLayout;
		void CreatePipelineLayout();
		void CreatePipeline();
		void RecordDrawCommands(uint32_t imageIndex);
		void RecreateSwapchain();
	};


} // namespace Helios
