#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"

#include <vulkan/vulkan.hpp>


namespace Helios {


	namespace Vulkan
	{
		class Instance;
		class Devices;
		class Swapchain;
		class Pipeline;
	}


	class VKRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void Shutdown() override;

	public:
		Ref<Vulkan::Instance>& GetInstance() { return m_Instance; }
		Ref<Vulkan::Devices>& GetDevices() { return m_Devices; }
		Ref<Vulkan::Swapchain>& GetSwapchain() { return m_Swapchain; }
		Ref<Vulkan::Pipeline>& GetPipeline() { return m_Pipeline; }

	private:
		Ref<Vulkan::Instance> m_Instance;
		Ref<Vulkan::Devices> m_Devices;
		Ref<Vulkan::Swapchain> m_Swapchain;
		Ref<Vulkan::Pipeline> m_Pipeline;
	};


} // namespace Helios
