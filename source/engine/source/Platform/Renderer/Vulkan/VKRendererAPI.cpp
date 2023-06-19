#include "pch.h"

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include "HeliosEngine/Core/Application.h"


namespace Helios {


	VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
		VkDebugUtilsMessageTypeFlagBitsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			LOG_RENDER_TRACE(pCallbackData->pMessage);
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			LOG_RENDER_INFO(pCallbackData->pMessage);
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			LOG_RENDER_WARN(pCallbackData->pMessage);
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			LOG_RENDER_ERROR(pCallbackData->pMessage);

		return VK_FALSE;
	}


	void VKRendererAPI::Init()
	{
		LOG_RENDER_DEBUG("Initializing vulkan renderer.");

		CreateInstance();

		m_vkLoader = vk::DispatchLoaderDynamic(m_vkInstance, vkGetInstanceProcAddr);

		#ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("Creating vulkan debug messenger.");
			CreateDebugMessanger();
		#endif
	}


	void VKRendererAPI::Shutdown()
	{
		LOG_RENDER_DEBUG("Shutting down vulkan renderer.");

		if (m_vkDebugMessenger)
			m_vkInstance.destroyDebugUtilsMessengerEXT(m_vkDebugMessenger, nullptr, m_vkLoader);

		if (m_vkInstance)
			m_vkInstance.destroy();
	}


	void VKRendererAPI::CreateInstance()
	{
		// Get Vulkan versions
		uint32_t vkVer;
		vkEnumerateInstanceVersion(&vkVer);
		LOG_RENDER_DEBUG("Systems supported vulkan version: {}.{}.{}.{}",
			VK_API_VERSION_VARIANT(vkVer),
			VK_API_VERSION_MAJOR(vkVer),
			VK_API_VERSION_MINOR(vkVer),
			VK_API_VERSION_PATCH(vkVer));
		LOG_RENDER_DEBUG("Engines supported vulkan version: {}.{}.{}.{}",
			VK_API_VERSION_VARIANT(VK_HEADER_VERSION_COMPLETE),
			VK_API_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
			VK_API_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
			VK_API_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE));

		// Setup AppInfo
		vk::ApplicationInfo vkAppInfo = vk::ApplicationInfo();
		{
			ApplicationSpecification appspec = Application::Get().GetSpecification();

			// app info
			vkAppInfo.setPApplicationName(appspec.Name.c_str());
			vkAppInfo.setApplicationVersion(appspec.Version);
			// engine info
			vkAppInfo.setPEngineName("HeliosEngine");
			vkAppInfo.setEngineVersion(HE_VERSION);
			// requested API version (just removed the patch-part of the supported version)
			vkAppInfo.setApiVersion(vkVer & ~(0xFFFU));
		}

		// Setup list of layers
		std::vector<const char*> layers;
		#ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("Using the \"VK_LAYER_KHRONOS_validation\" layer.");
			layers.push_back("VK_LAYER_KHRONOS_validation");
		#endif
		// Check support
		LOG_RENDER_ASSERT(CheckSupportedLayers(layers), "Required layers not supported!");

		// Get required extensions for GLFW
		uint32_t glfwExtCount = 0;
		const char** glfwExt;
		glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);
		// Setup list of extensions
		std::vector<const char*> extensions(glfwExt, glfwExt + glfwExtCount);
		#ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("Using the \"VK_EXT_debug_utils\" extension.");
			extensions.push_back("VK_EXT_debug_utils");
		#endif
		// Check support
		LOG_RENDER_ASSERT(CheckSupportedExtensions(extensions), "Required extensions not supported!");

		// Setup InstanceInfo
		vk::InstanceCreateInfo vkCreateInfo = vk::InstanceCreateInfo();
		{
			// app info
			vkCreateInfo.setPApplicationInfo(&vkAppInfo);
			// layers
			vkCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(layers.size()));
			vkCreateInfo.setPpEnabledLayerNames(layers.data());
			// extensions
			vkCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
			vkCreateInfo.setPpEnabledExtensionNames(extensions.data());
		}

		// Create the vulkan instance
		try {
			m_vkInstance = vk::createInstance(vkCreateInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_ERROR("Failed to create vulkan instance!");
			LOG_RENDER_ASSERT(0, "");
		}
	}


	void VKRendererAPI::CreateDebugMessanger()
	{
		vk::DebugUtilsMessengerCreateInfoEXT vkCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT();
		vkCreateInfo.setMessageSeverity(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
		vkCreateInfo.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding);
		vkCreateInfo.setPfnUserCallback((PFN_vkDebugUtilsMessengerCallbackEXT)VKDebugCallback);

		m_vkDebugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(vkCreateInfo, nullptr, m_vkLoader);
	}


	bool VKRendererAPI::CheckSupportedLayers(std::vector<const char*>& layers)
	{
		std::vector<vk::LayerProperties> vkSupported = vk::enumerateInstanceLayerProperties();

		bool found;
		for (auto layer : layers)
		{
			found = false;
			for (auto suppLayer : vkSupported)
			{
				if (strcmp(layer, suppLayer.layerName) == 0)
					found = true;
			}
			if (!found)
			{
				LOG_RENDER_FATAL("Required extension \"{}\" is not supported!", layer);
				return false;
			}
		}

		return true;
	}


	bool VKRendererAPI::CheckSupportedExtensions(std::vector<const char*>& extensions)
	{
		std::vector<vk::ExtensionProperties> vkSupported = vk::enumerateInstanceExtensionProperties();

		bool found;
		for (auto ext : extensions)
		{
			found = false;
			for (auto suppExt : vkSupported)
			{
				if (strcmp(ext, suppExt.extensionName) == 0)
					found = true;
			}
			if (!found)
			{
				LOG_RENDER_FATAL("Required extension \"{}\" is not supported!", ext);
				return false;
			}
		}

		return true;
	}


} // namespace Helios
