#include "pch.h"

#include "Platform/Renderer/Vulkan/VKSwapChain.h"

#include <GLFW/glfw3.h>

#include "HeliosEngine/Core/Application.h"


namespace Helios {


	VKSwapChain::VKSwapChain(Ref<VKInstance>& inst, Ref<VKDevice>& dev)
		: m_Instance(inst), m_Device(dev)
	{
	}


	void VKSwapChain::Create()
	{
		LOG_RENDER_DEBUG("Creating vulkan swapchain...");

		// Get some capabilities
		vk::SurfaceCapabilitiesKHR capabilities = m_Device->GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Instance->GetSurface());
		QueueFamilyIndices indices = m_Device->FindQueueFamilies(m_Device->GetPhysicalDevice());
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		// Setup CreateInfo
		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR();
		{
			createInfo.surface = m_Instance->GetSurface();
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
			m_vkSwapChain = m_Device->GetLogicalDevice().createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_ASSERT(0, "Failed to create swapchain!");
		}

		// Save some data
		m_vkFormat = createInfo.imageFormat;
		m_vkExtent = createInfo.imageExtent;

		// Create views
		std::vector<vk::Image> images = m_Device->GetLogicalDevice().getSwapchainImagesKHR(m_vkSwapChain);
		m_Frames.resize(images.size());
		for (auto i = 0; i < images.size(); ++i)
		{
			// Setup ViewInfo
			vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo();
			{
				viewInfo.image = images[i];
				viewInfo.viewType = vk::ImageViewType::e2D;
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
				viewInfo.format = m_vkFormat;
			}
			// Create
			m_Frames[i].image = images[i];
			m_Frames[i].view = m_Device->GetLogicalDevice().createImageView(viewInfo);
		}
	}


	void VKSwapChain::Destroy()
	{
		for (auto frame : m_Frames)
			m_Device->GetLogicalDevice().destroyImageView(frame.view);
		if (m_vkSwapChain)
			m_Device->GetLogicalDevice().destroySwapchainKHR(m_vkSwapChain);
	}


#if 0
	void VKSwapChain::QuerySupport()
	{
		vk::SurfaceCapabilitiesKHR capabilities = m_Device->GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Instance->GetSurface());
		std::vector<vk::SurfaceFormatKHR> formats = m_Device->GetPhysicalDevice().getSurfaceFormatsKHR(m_Instance->GetSurface());
		std::vector<vk::PresentModeKHR> presentModes = m_Device->GetPhysicalDevice().getSurfacePresentModesKHR(m_Instance->GetSurface());

		if (LOG_LEVEL <= LOG_LEVEL_TRACE)
		{
			LOG_RENDER_TRACE("Surface capabilities:");
			LOG_RENDER_TRACE("- Image count: {}..{}", capabilities.minImageCount, capabilities.maxImageCount);
			LOG_RENDER_TRACE("- Current extent: {}/{}", capabilities.currentExtent.width, capabilities.currentExtent.height);
			LOG_RENDER_TRACE("- Min. image ext: {}/{}", capabilities.minImageExtent.width, capabilities.minImageExtent.height);
			LOG_RENDER_TRACE("- Max. image ext: {}/{}", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);

			LOG_RENDER_TRACE("- Current transform:");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eIdentity)
				LOG_RENDER_TRACE("  - Identity");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eRotate90)
				LOG_RENDER_TRACE("  - Rotate90");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eRotate180)
				LOG_RENDER_TRACE("  - Rotate180");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eRotate270)
				LOG_RENDER_TRACE("  - Rotate270");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror)
				LOG_RENDER_TRACE("  - HorizontalMirror");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90)
				LOG_RENDER_TRACE("  - HorizontalMirrorRotate90");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180)
				LOG_RENDER_TRACE("  - HorizontalMirrorRotate180");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270)
				LOG_RENDER_TRACE("  - HorizontalMirrorRotate270");
			if (capabilities.currentTransform & vk::SurfaceTransformFlagBitsKHR::eInherit)
				LOG_RENDER_TRACE("  - Inherit");

			LOG_RENDER_TRACE("- Supported transform:");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
				LOG_RENDER_TRACE("  - Identity");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eRotate90)
				LOG_RENDER_TRACE("  - Rotate90");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eRotate180)
				LOG_RENDER_TRACE("  - Rotate180");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eRotate270)
				LOG_RENDER_TRACE("  - Rotate270");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror)
				LOG_RENDER_TRACE("  - HorizontalMirror");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90)
				LOG_RENDER_TRACE("  - HorizontalMirrorRotate90");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180)
				LOG_RENDER_TRACE("  - HorizontalMirrorRotate180");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270)
				LOG_RENDER_TRACE("  - HorizontalMirrorRotate270");
			if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eInherit)
				LOG_RENDER_TRACE("  - Inherit");

			LOG_RENDER_TRACE("- Supported composite alpha:");
			if (capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque)
				LOG_RENDER_TRACE("  - Opaque");
			if (capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
				LOG_RENDER_TRACE("  - PreMultiplied");
			if (capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
				LOG_RENDER_TRACE("  - PostMultiplied");
			if (capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
				LOG_RENDER_TRACE("  - Inherit");

			LOG_RENDER_TRACE("- Supported image usage:");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc)
				LOG_RENDER_TRACE("  - TransferSrc");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst)
				LOG_RENDER_TRACE("  - TransferDst");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eSampled)
				LOG_RENDER_TRACE("  - Sampled");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eStorage)
				LOG_RENDER_TRACE("  - Storage");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eColorAttachment)
				LOG_RENDER_TRACE("  - ColorAttachment");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
				LOG_RENDER_TRACE("  - DepthStencilAttachment");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransientAttachment)
				LOG_RENDER_TRACE("  - TransientAttachment");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eInputAttachment)
				LOG_RENDER_TRACE("  - InputAttachment");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eVideoDecodeDstKHR)
				LOG_RENDER_TRACE("  - VideoDecodeDstKHR");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eVideoDecodeSrcKHR)
				LOG_RENDER_TRACE("  - VideoDecodeSrcKHR");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eVideoDecodeDpbKHR)
				LOG_RENDER_TRACE("  - VideoDecodeDpbKHR");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eShadingRateImageNV)
				LOG_RENDER_TRACE("  - ShadingRateImageNV");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eFragmentDensityMapEXT)
				LOG_RENDER_TRACE("  - FragmentDensityMapEXT");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR)
				LOG_RENDER_TRACE("  - FragmentShadingRateAttachmentKHR");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eShadingRateImageNV)
				LOG_RENDER_TRACE("  - ShadingRateImageNV");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eFragmentDensityMapEXT)
				LOG_RENDER_TRACE("  - FragmentDensityMapEXT");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR)
				LOG_RENDER_TRACE("  - FragmentShadingRateAttachmentKHR");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eAttachmentFeedbackLoopEXT)
				LOG_RENDER_TRACE("  - AttachmentFeedbackLoopEXT");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eInvocationMaskHUAWEI)
				LOG_RENDER_TRACE("  - InvocationMaskHUAWEI");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eSampleWeightQCOM)
				LOG_RENDER_TRACE("  - SampleWeightQCOM");
			if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eSampleBlockMatchQCOM)
				LOG_RENDER_TRACE("  - SampleBlockMatchQCOM");

			LOG_RENDER_TRACE("Supported surface formats:");
			for (auto format : formats)
			{
				LOG_RENDER_TRACE("- Pixel format: {}", vk::to_string(format.format));
				LOG_RENDER_TRACE("- Color space:  {}", vk::to_string(format.colorSpace));
			}

			LOG_RENDER_TRACE("Supported present modes:");
			for (auto mode : presentModes)
			{
				if (mode == vk::PresentModeKHR::eImmediate)
					LOG_RENDER_TRACE("- Immediate");
				if (mode == vk::PresentModeKHR::eMailbox)
					LOG_RENDER_TRACE("- Mailbox");
				if (mode == vk::PresentModeKHR::eFifo)
					LOG_RENDER_TRACE("- Fifo");
				if (mode == vk::PresentModeKHR::eFifoRelaxed)
					LOG_RENDER_TRACE("- FifoRelaxed");
				if (mode == vk::PresentModeKHR::eSharedDemandRefresh)
					LOG_RENDER_TRACE("- SharedDemandRefresh");
				if (mode == vk::PresentModeKHR::eSharedContinuousRefresh)
					LOG_RENDER_TRACE("- SharedContinuousRefresh");
			}
		}
	}
#endif


	vk::SurfaceFormatKHR VKSwapChain::ChooseSurfaceFormat()
	{
		std::vector<vk::SurfaceFormatKHR> formats = m_Device->GetPhysicalDevice().getSurfaceFormatsKHR(m_Instance->GetSurface());

		// Try eB8G8R8A8Unorm which is the common standard
		// Try eSrgbNonlinear for best quality
		formats = m_Device->GetPhysicalDevice().getSurfaceFormatsKHR(m_Instance->GetSurface());
		for (auto format : formats)
		{
			if ((format.format == vk::Format::eB8G8R8A8Unorm) and (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;
		}

		return formats[0];
	}


	vk::PresentModeKHR VKSwapChain::ChoosePresentMode()
	{
		std::vector<vk::PresentModeKHR> modes = m_Device->GetPhysicalDevice().getSurfacePresentModesKHR(m_Instance->GetSurface());

		// Try eMailbox for fastest framerate
		for (auto mode : modes)
		{
			if (mode == vk::PresentModeKHR::eMailbox)
				return mode;
		}

		return vk::PresentModeKHR::eFifo;
	}


	vk::Extent2D VKSwapChain::ChooseExtent()
	{
		vk::SurfaceCapabilitiesKHR capabilities = m_Device->GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Instance->GetSurface());

		// If set just return the (fixed) current extent
		if (capabilities.currentExtent.width != UINT32_MAX)
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


} // namespace Helios
