#pragma once

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


namespace Helios::Vulkan {


	struct FrameInfo
	{
		vk::Image image;
		vk::ImageView view;
	};


	class Instance;
	class Devices;


	class Swapchain
	{
	public:
		Swapchain();
		~Swapchain() = default;

		void Create();
		void Destroy();

	public:
		vk::Extent2D GetExtent() { return m_vkSwapchainExtent; }
		vk::Format GetFormat() { return m_vkSwapchainFormat; }

	private:
		vk::SurfaceFormatKHR ChooseSurfaceFormat();
		vk::PresentModeKHR ChoosePresentMode();
		vk::Extent2D ChooseExtent();
		void QuerySwapchainSupport();

	private:
		// Class/Object pointers
		Ref<Instance> m_Instance;
		Ref<Devices> m_Devices;

		// Vulkan objects
		vk::SwapchainKHR m_vkSwapchain;
		vk::Format m_vkSwapchainFormat = {};
		vk::Extent2D m_vkSwapchainExtent;

		// Internal data
		std::vector<FrameInfo> m_Frames;
	};


} // namespace Helios::Vulkan
