#pragma once

#include <vulkan/vulkan.hpp>

namespace Helios::Vulkan {


	class Swapchain
	{
	public:
		Swapchain();
		Swapchain(Ref<Swapchain> oldSwapchain);
		~Swapchain();

		void Create();
		void Destroy();

	// Getter for vulkan objects
	public:
		vk::SwapchainKHR& GetSwapchain() { return m_vkSwapchain; }
		vk::Extent2D& GetExtent() { return m_vkSwapchainExtent; }
		vk::Format& GetImageFormat() { return m_vkSwapchainImageFormat; }
		vk::RenderPass& GetRenderPass() { return m_vkRenderPass; }

		vk::Framebuffer& GetFrameBuffer(uint32_t index) { return m_frameBuffers[index]; }
		vk::CommandBuffer& GetCommandBuffer(uint32_t index) { return m_commandBuffers[index]; }

		vk::Result AcquireNextFrameIndex(uint32_t *imageIndex);
		vk::Result SubmitCommandBuffer(uint32_t imageIndex);

	// Vulkan objects
	private:
		vk::SwapchainKHR m_vkSwapchain;
		Ref<Swapchain> m_oldSwapchain;
		vk::Extent2D m_vkSwapchainExtent;
		vk::Format m_vkSwapchainImageFormat;
		vk::Format m_vkSwapchainDepthFormat;
		vk::RenderPass m_vkRenderPass;

	// Internal helper
	private:
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateSyncObjects();
		void CreateCommandBuffers();

		vk::SurfaceFormatKHR ChooseSurfaceFormat();
		vk::PresentModeKHR ChoosePresentMode();
		vk::Extent2D ChooseExtent();
//		void QuerySwapchainSupport();
		vk::Format QueryDepthFormat();

	// Internal data
	private:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		// Frame objects
		std::vector<vk::Framebuffer> m_frameBuffers;
		std::vector<vk::Image> m_frameImages;
		std::vector<vk::ImageView> m_frameImageViews;
		std::vector<vk::Image> m_depthImages;
		std::vector<vk::ImageView> m_depthImageViews;
		std::vector<vk::DeviceMemory> m_depthImageMemories;
		std::vector<vk::CommandBuffer> m_commandBuffers;

		// Sync objects
		std::vector<vk::Semaphore> m_imagesAvailable;
		std::vector<vk::Semaphore> m_renderFinished;
		std::vector<vk::Fence> m_inFlightFences;
		std::vector<vk::Fence> m_imagesInFlight;
		uint32_t m_currentFrame = 0;
	};


} // namespace Helios::Vulkan
