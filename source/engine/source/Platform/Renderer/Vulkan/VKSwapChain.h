#pragma once

#include <vulkan/vulkan.hpp>

#include "Platform/Renderer/Vulkan/VKInstance.h"
#include "Platform/Renderer/Vulkan/VKDevice.h"


namespace Helios {

	class VKSwapChain
	{
	public:
		VKSwapChain(Ref<VKInstance>& inst, Ref<VKDevice>& dev);

		void Create();
		void Destroy();

	private:
//		void QuerySupport();
		vk::SurfaceFormatKHR ChooseSurfaceFormat();
		vk::PresentModeKHR ChoosePresentMode();
		vk::Extent2D ChooseExtent();

	private:
		// Native vulkan objects
		vk::SwapchainKHR m_vkSwapChain;
		std::vector<vk::Image> m_vkImages;
		vk::Format m_vkFormat;
		vk::Extent2D m_vkExtent;

		// Internal data
		Ref<VKInstance> m_Instance;
		Ref<VKDevice> m_Device;
	};


} // namespace Helios
