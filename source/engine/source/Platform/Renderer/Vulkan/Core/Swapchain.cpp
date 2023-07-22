#include "pch.h"
#include "Swapchain.h"

#include "HeliosEngine/Renderer/Renderer.h"
#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include "HeliosEngine/Core/Application.h"


namespace Helios::Vulkan {


	Swapchain::Swapchain()
	{
		Create();
	}


	Swapchain::Swapchain(Ref<Swapchain> oldSwapchain)
		: m_oldSwapchain{oldSwapchain}
	{
		Create();
		m_oldSwapchain.reset();
	}


	Swapchain::~Swapchain()
	{
		Destroy();
	}


	void Swapchain::Create()
	{
		LOG_RENDER_TRACE("Creating swapchain objects...");

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResources();
		CreateFrameBuffers();
		CreateSyncObjects();
		CreateCommandBuffers();
	}


	void Swapchain::Destroy()
	{
		Scope<Device>& device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		LOG_RENDER_TRACE("Destroying swapchain objects...");

		for (auto imageView : m_frameImageViews)
			device->GetLogicalDevice().destroyImageView(imageView);
		m_frameImageViews.clear();

		for (auto i = 0; i < m_depthImages.size(); i++)
		{
			device->GetLogicalDevice().destroyImageView(m_depthImageViews[i]);
			device->GetLogicalDevice().destroyImage(m_depthImages[i]);
			device->GetLogicalDevice().freeMemory(m_depthImageMemories[i]);
		}

		for (auto framebuffer : m_frameBuffers)
			device->GetLogicalDevice().destroyFramebuffer(framebuffer);

		for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			device->GetLogicalDevice().destroyFence(m_inFlightFences[i]);
			device->GetLogicalDevice().destroySemaphore(m_imagesAvailable[i]);
			device->GetLogicalDevice().destroySemaphore(m_renderFinished[i]);
		}

		if (m_vkRenderPass)
			device->GetLogicalDevice().destroyRenderPass(m_vkRenderPass);

		if (m_vkSwapchain)
			device->GetLogicalDevice().destroySwapchainKHR(m_vkSwapchain);
	}


	void Swapchain::CreateSwapChain()
	{
		Scope<Instance> &instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		// Get some capabilities
		vk::SurfaceCapabilitiesKHR capabilities = device->GetPhysicalDevice().getSurfaceCapabilitiesKHR(instance->GetSurface());
		QueueFamilyIndices indices = device->QueryQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		// Setup CreateInfo
		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR();
		{
			createInfo.surface = instance->GetSurface();
			createInfo.minImageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount ? capabilities.maxImageCount : UINT32_MAX);
			createInfo.imageFormat = ChooseSurfaceFormat().format;
			createInfo.imageColorSpace = ChooseSurfaceFormat().colorSpace;
			createInfo.imageExtent = ChooseExtent();
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
			createInfo.preTransform = capabilities.currentTransform;
			createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
			createInfo.presentMode = ChoosePresentMode();
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = m_oldSwapchain ? m_oldSwapchain->GetSwapchain() : VK_NULL_HANDLE;

			if (indices.graphicsFamily.value() != indices.presentFamily.value())
			{
				createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else
				createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		// Create the swapchain
		try {
			LOG_RENDER_TRACE("Creating swapchain...");
			m_vkSwapchain = device->GetLogicalDevice().createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create swapchain!");
		}
		m_frameImages = device->GetLogicalDevice().getSwapchainImagesKHR(m_vkSwapchain);

		m_vkSwapchainImageFormat = createInfo.imageFormat;
		m_vkSwapchainExtent = createInfo.imageExtent;
	}


	void Swapchain::CreateImageViews()
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		m_frameImageViews.resize(m_frameImages.size());
		for (auto i = 0; i < m_frameImages.size(); ++i)
		{
			// Setup ViewInfo
			vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo();
			{
				viewInfo.image = m_frameImages[i];
				viewInfo.viewType = vk::ImageViewType::e2D;
				viewInfo.format = m_vkSwapchainImageFormat;

				vk::ComponentMapping comp;
				comp.r = vk::ComponentSwizzle::eIdentity;
				comp.g = vk::ComponentSwizzle::eIdentity;
				comp.b = vk::ComponentSwizzle::eIdentity;
				comp.a = vk::ComponentSwizzle::eIdentity;
				viewInfo.components = comp;

				vk::ImageSubresourceRange range;
				range.aspectMask = vk::ImageAspectFlagBits::eColor;
				range.baseMipLevel = 0;
				range.levelCount = 1;
				range.baseArrayLayer = 0;
				range.layerCount = 1;
				viewInfo.subresourceRange = range;
			}
			// Create
			try {
				LOG_RENDER_TRACE("Creating image views for frame #{}...", i);
				m_frameImageViews[i] = device->GetLogicalDevice().createImageView(viewInfo);
			}
			catch (vk::SystemError err) {
				LOG_RENDER_EXCEPT("Failed to create image view!");
			}
		}
	}


	void Swapchain::CreateDepthResources()
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		m_depthImages.resize(m_frameImages.size());
		m_depthImageMemories.resize(m_frameImages.size());
		m_depthImageViews.resize(m_frameImages.size());

		m_vkSwapchainDepthFormat = QueryDepthFormat();

		for (auto i = 0; i < m_depthImages.size(); ++i)
		{
			vk::ImageCreateInfo imageInfo = vk::ImageCreateInfo();
			{
				imageInfo.imageType = vk::ImageType::e2D;
				imageInfo.extent.width = m_vkSwapchainExtent.width;
				imageInfo.extent.height = m_vkSwapchainExtent.height;
				imageInfo.extent.depth = 1;
				imageInfo.mipLevels = 1;
				imageInfo.arrayLayers = 1;
				imageInfo.format = m_vkSwapchainDepthFormat;
				imageInfo.tiling = vk::ImageTiling::eOptimal;
				imageInfo.initialLayout = vk::ImageLayout::eUndefined;
				imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
				imageInfo.samples = vk::SampleCountFlagBits::e1;
				imageInfo.sharingMode = vk::SharingMode::eExclusive;
			}
			device->CreateImageWithMemory(imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal,
					m_depthImages[i], m_depthImageMemories[i]);

			vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo();
			{
				viewInfo.image = m_depthImages[i];
				viewInfo.viewType = vk::ImageViewType::e2D;
				viewInfo.format = m_vkSwapchainDepthFormat;
				viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
				viewInfo.subresourceRange.baseMipLevel = 0;
				viewInfo.subresourceRange.levelCount = 1;
				viewInfo.subresourceRange.baseArrayLayer = 0;
				viewInfo.subresourceRange.layerCount = 1;
			}

			try {
				LOG_RENDER_TRACE("Creating depth view for frame #{}...", i);
				m_depthImageViews[i] = device->GetLogicalDevice().createImageView(viewInfo);
			}
			catch (vk::SystemError err) {
				LOG_RENDER_EXCEPT("Failed to create depth view!");
			}
		}
	}


	void Swapchain::CreateRenderPass()
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		vk::AttachmentDescription colorAttachment = vk::AttachmentDescription();
		{
			colorAttachment.format = m_vkSwapchainImageFormat;
			colorAttachment.samples = vk::SampleCountFlagBits::e1;
			colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
			colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
			colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
			colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		}
		vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference();
		{
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
		}
		vk::AttachmentDescription depthAttachment = vk::AttachmentDescription();
		{
			depthAttachment.format = QueryDepthFormat();
			depthAttachment.samples = vk::SampleCountFlagBits::e1;
			depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
			depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
			depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
			depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		}
		vk::AttachmentReference depthAttachmentRef = vk::AttachmentReference();
		{
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		}
		vk::SubpassDescription subpass = vk::SubpassDescription();
		{
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
		}
		vk::SubpassDependency dependency = vk::SubpassDependency();
		{
			dependency.dstSubpass = 0;
			dependency.dstAccessMask =
				vk::AccessFlagBits::eColorAttachmentWrite |
				vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			dependency.dstStageMask =
				vk::PipelineStageFlagBits::eColorAttachmentOutput |
				vk::PipelineStageFlagBits::eEarlyFragmentTests;
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.srcAccessMask = vk::AccessFlagBits::eNone;
			dependency.srcStageMask =
				vk::PipelineStageFlagBits::eColorAttachmentOutput |
				vk::PipelineStageFlagBits::eEarlyFragmentTests;
		}
		vk::RenderPassCreateInfo renderPass = vk::RenderPassCreateInfo();
		{
			std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
			renderPass.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPass.pAttachments = attachments.data();
			renderPass.subpassCount = 1;
			renderPass.pSubpasses = &subpass;
			renderPass.dependencyCount = 1;
			renderPass.pDependencies = &dependency;
		}

		try {
			LOG_RENDER_TRACE("Creating render pass...");
			m_vkRenderPass = device->GetLogicalDevice().createRenderPass(renderPass);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create render pass!");
		}
	}


	void Swapchain::CreateFrameBuffers()
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		m_frameBuffers.resize(m_frameImages.size());
		for (auto i = 0; i < m_frameImages.size(); ++i)
		{
			std::vector<vk::ImageView> attachments = {
				m_frameImageViews[i],
				m_depthImageViews[i]
			};

			vk::FramebufferCreateInfo bufferInfo = vk::FramebufferCreateInfo();
			{
				bufferInfo.renderPass = m_vkRenderPass;
				bufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				bufferInfo.pAttachments = attachments.data();
				bufferInfo.width = m_vkSwapchainExtent.width;
				bufferInfo.height = m_vkSwapchainExtent.height;
				bufferInfo.layers = 1;
			}

			try {
				LOG_RENDER_TRACE("Creating framebuffer for frame #{}...", i);
				m_frameBuffers[i] = device->GetLogicalDevice().createFramebuffer(bufferInfo);
			}
			catch (vk::SystemError err) {
				LOG_RENDER_EXCEPT("Failed to create framebuffer!");
			}
		}
	}


	void Swapchain::CreateSyncObjects()
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		m_imagesAvailable.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinished.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_imagesInFlight.resize(m_frameImages.size(), VK_NULL_HANDLE);

		vk::SemaphoreCreateInfo semInfo = vk::SemaphoreCreateInfo();
		{
		}
		vk::FenceCreateInfo fenceInfo = vk::FenceCreateInfo();
		{
			fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		}

		for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			try {
				LOG_RENDER_TRACE("Creating sync objects for index #{}...", i);
				m_imagesAvailable[i] = device->GetLogicalDevice().createSemaphore(semInfo);
				m_renderFinished[i] = device->GetLogicalDevice().createSemaphore(semInfo);
				m_inFlightFences[i] = device->GetLogicalDevice().createFence(fenceInfo);
			}
			catch (vk::SystemError err) {
				LOG_RENDER_EXCEPT("Failed to create frame sync objects!");
			}
		}
	}


	void Swapchain::CreateCommandBuffers()
	{
		Scope<Device>& device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

//		m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		m_commandBuffers.resize(m_frameImages.size());

		vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo();
		{
			allocInfo.level = vk::CommandBufferLevel::ePrimary;
			allocInfo.commandPool = device->GetCommandPool();
			allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
		}
		try {
			LOG_RENDER_TRACE("Allocating command buffers...");
			m_commandBuffers = device->GetLogicalDevice().allocateCommandBuffers(allocInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to allocate command buffer!");
		}
	}


	vk::Result Swapchain::AcquireNextFrameIndex(uint32_t* imageIndex)
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		auto resultWait = device->GetLogicalDevice().waitForFences(
			1,
			&m_inFlightFences[m_currentFrame],
			VK_TRUE,
			std::numeric_limits<uint64_t>::max());

		auto result = device->GetLogicalDevice().acquireNextImageKHR(
			m_vkSwapchain,
			std::numeric_limits<uint64_t>::max(),
			m_imagesAvailable[m_currentFrame], VK_NULL_HANDLE, imageIndex);

		return result;
	}


	vk::Result Swapchain::SubmitCommandBuffer(uint32_t imageIndex)
	{
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		if (m_imagesInFlight[imageIndex])
		{
			auto resultWait = device->GetLogicalDevice().waitForFences(
				1,
				&m_imagesInFlight[imageIndex],
				VK_TRUE,
				std::numeric_limits<uint64_t>::max());
		}
		m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

		vk::Semaphore waitSemaphores[] = { m_imagesAvailable[m_currentFrame] };
		vk::Semaphore signalSemaphores[] = { m_renderFinished[m_currentFrame] };

		vk::SubmitInfo submitInfo = vk::SubmitInfo();
		{
			vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;
		}
		auto resultReset = device->GetLogicalDevice().resetFences(1, &m_inFlightFences[m_currentFrame]);
		try {
			device->GetGraphicsQueue().submit(submitInfo, m_inFlightFences[m_currentFrame]);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to submit command buffer!");
		}

		uint32_t indices = imageIndex;
		vk::PresentInfoKHR PresentInfo = {};
		{
			vk::SwapchainKHR swapchains[] = { m_vkSwapchain };
			PresentInfo.waitSemaphoreCount = 1;
			PresentInfo.pWaitSemaphores = signalSemaphores;
			PresentInfo.swapchainCount = 1;
			PresentInfo.pSwapchains = swapchains;
			PresentInfo.pImageIndices = &indices;
		}

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		vk::Result resultPresent;
		try {
			resultPresent = device->GetPresentQueue().presentKHR(PresentInfo);
		}
		catch (vk::SystemError err) {
			return vk::Result::eErrorOutOfDateKHR;
		}
		return resultPresent;
	}


	vk::SurfaceFormatKHR Swapchain::ChooseSurfaceFormat()
	{
		Scope<Instance> &instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		std::vector<vk::SurfaceFormatKHR> formats = device->GetPhysicalDevice().getSurfaceFormatsKHR(instance->GetSurface());

		// First try: eR8G8B8A8Srgb which is the common standard with gamma correction
		// First try: eSrgbNonlinear for best quality
		formats = device->GetPhysicalDevice().getSurfaceFormatsKHR(instance->GetSurface());
		for (auto format : formats)
		{
			if ((format.format == vk::Format::eR8G8B8A8Srgb) and (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;
		}

		// Second try: eB8G8R8A8Unorm which is the common standard without gamma correction
		// Second try: eSrgbNonlinear for best quality
		formats = device->GetPhysicalDevice().getSurfaceFormatsKHR(instance->GetSurface());
		for (auto format : formats)
		{
			if ((format.format == vk::Format::eB8G8R8A8Unorm) and (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;
		}

		// Last: just the first found
		return formats[0];
	}
	

	vk::PresentModeKHR Swapchain::ChoosePresentMode()
	{
		Scope<Instance> &instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		std::vector<vk::PresentModeKHR> modes = device->GetPhysicalDevice().getSurfacePresentModesKHR(instance->GetSurface());

#if 1
		// Try eMailbox for fastest framerate
		for (auto mode : modes)
		{
			if (mode == vk::PresentModeKHR::eMailbox)
			{
				LOG_RENDER_DEBUG("Choosen present mode: Mailbox");
				return mode;
			}
		}
#endif
#if 0
		// Try eImmediate for medium framerate
		for (auto mode : modes)
		{
			if (mode == vk::PresentModeKHR::eImmediate)
			{
				LOG_RENDER_DEBUG("Choosen present mode: Immediate");
				return mode;
			}
		}
#endif

		// Try eFifo as fallback
		LOG_RENDER_DEBUG("Choosen present mode: Fifo (V-Sync)");
		return vk::PresentModeKHR::eFifo;
	}


	vk::Extent2D Swapchain::ChooseExtent()
	{
		Scope<Instance> &instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();
		Scope<Device> &device = static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice();

		vk::SurfaceCapabilitiesKHR capabilities = device->GetPhysicalDevice().getSurfaceCapabilitiesKHR(instance->GetSurface());

		// If set just return the (fixed) current extent
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		// Get the window/framebuffer pixel size
		int width, height;
		glfwGetFramebufferSize((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), &width, &height);

		// Calculate dimensions
		vk::Extent2D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return extent;
	}


	vk::Format Swapchain::QueryDepthFormat()
	{
		return static_cast<VKRendererAPI*>(Renderer::Get())->GetDevice()->QuerySupportedFormat(
			{
				vk::Format::eD32Sfloat,
				vk::Format::eD32SfloatS8Uint,
				vk::Format::eD24UnormS8Uint
			},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);
	}


} // namespace Helios::Vulkan
