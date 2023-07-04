#include "pch.h"

#include "Platform/Renderer/Vulkan/Core/Instance.h"

#include <GLFW/glfw3.h>

#include "HeliosEngine/Core/Application.h"


namespace Helios::Vulkan {


	VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagBitsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT

		// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT

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


	void Instance::Create()
	{
		LOG_RENDER_DEBUG("Creating vulkan instance...");

		// Get vulkan versions
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
		vk::ApplicationInfo appInfo = vk::ApplicationInfo();
		{
			appInfo.pApplicationName = Application::Get().GetSpecification().Name.c_str();
			appInfo.applicationVersion = Application::Get().GetSpecification().Version;
			appInfo.pEngineName = "HeliosEngine";
			appInfo.engineVersion = HE_VERSION;
			appInfo.apiVersion = VK_API_VERSION_1_1;
		}

		// Setup list of layers
#		ifdef BUILD_DEBUG
			m_ListLayers.push_back("VK_LAYER_KHRONOS_validation");
#		endif

				// Check layer support
		if (!CheckSupportedLayers())
		{
			LOG_RENDER_FATAL("Required instance layers not supported!");
			return;
		}

		// Get required extensions for GLFW
		uint32_t glfwExtCount = 0;
		const char** glfwExt;
		glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);
		m_ListExtensions = std::vector<const char*>(glfwExt, glfwExt + glfwExtCount);

		// Setup list of extensions
#		ifdef BUILD_DEBUG
			m_ListExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#		endif

				// Check extension support
		if (!CheckSupportedExtensions())
		{
			LOG_RENDER_FATAL("Required instance extensions not supported!");
			return;
		}

		// Setup InstanceInfo
		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo();
		{
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ListLayers.size());
			createInfo.ppEnabledLayerNames = m_ListLayers.data();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(m_ListExtensions.size());
			createInfo.ppEnabledExtensionNames = m_ListExtensions.data();
		}

		// Create the vulkan instance
		try {
			m_vkInstance = vk::createInstance(createInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_FATAL("Failed to create instance!");
			return;
		}

		// Get the function loader
		m_vkLoader = vk::DispatchLoaderDynamic(m_vkInstance, vkGetInstanceProcAddr);

		// Create debug callback
#		ifdef BUILD_DEBUG
			CreateDebugMessanger();
#		endif

				// Create the surface
		CreateSurface();
	}


	void Instance::Destroy()
	{
		if (m_vkSurface)
			m_vkInstance.destroySurfaceKHR(m_vkSurface);

		if (m_vkDebugMessenger)
			m_vkInstance.destroyDebugUtilsMessengerEXT(m_vkDebugMessenger, nullptr, m_vkLoader);

		if (m_vkInstance)
			m_vkInstance.destroy();
	}


	bool Instance::CheckSupportedLayers()
	{
		std::vector<vk::LayerProperties> supported = vk::enumerateInstanceLayerProperties();
		std::set<std::string> required(m_ListLayers.begin(), m_ListLayers.end());

		if (LOG_LEVEL < LOG_LEVEL_DEBUG)
			LOG_RENDER_TRACE("Supported instance layers ({}):", supported.size());
		else
			LOG_RENDER_DEBUG("Required instance layers ({}/{}):", required.size(), supported.size());
		for (auto supp : supported)
		{
			if (required.contains(supp.layerName))
				LOG_RENDER_DEBUG("[  OK  ] \"{}\"", supp.layerName);
			else
				LOG_RENDER_TRACE("[UNUSED] \"{}\"", supp.layerName);
			required.erase(supp.layerName);
		}
		if (!required.empty())
			LOG_RENDER_DEBUG("Unsupported instance layers ({}):", required.size());
		for (auto req : required)
			LOG_RENDER_DEBUG("[FAILED] \"{}\"", req);

		return required.empty();
	}


	bool Instance::CheckSupportedExtensions()
	{
		std::vector<vk::ExtensionProperties> supported = vk::enumerateInstanceExtensionProperties();
		std::set<std::string> required(m_ListExtensions.begin(), m_ListExtensions.end());

		if (LOG_LEVEL < LOG_LEVEL_DEBUG)
			LOG_RENDER_TRACE("Supported instance extensions ({}):", supported.size());
		else
			LOG_RENDER_DEBUG("Required instance extensions ({}/{}):", required.size(), supported.size());
		for (auto supp : supported)
		{
			if (required.contains(supp.extensionName))
				LOG_RENDER_DEBUG("[  OK  ] \"{}\"", supp.extensionName);
			else
				LOG_RENDER_TRACE("[UNUSED] \"{}\"", supp.extensionName);
			required.erase(supp.extensionName);
		}
		if (!required.empty())
			LOG_RENDER_DEBUG("Unsupported instance extensions ({}):", required.size());
		for (auto req : required)
			LOG_RENDER_DEBUG("[FAILED] \"{}\"", req);

		return required.empty();
	}


	void Instance::CreateDebugMessanger()
	{
		vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT();
		{
			createInfo.messageSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
			createInfo.messageType =
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
			createInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)VKDebugCallback;
		}

		m_vkDebugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_vkLoader);
		LOG_RENDER_ASSERT(m_vkDebugMessenger, "Failed to create debug messenger!");
	}


	void Instance::CreateSurface()
	{
		VkSurfaceKHR c_style_suface;
		if (glfwCreateWindowSurface(m_vkInstance, (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &c_style_suface) != VK_SUCCESS)
		{
			LOG_RENDER_FATAL("Failed to create surface!");
			return;
		}

		m_vkSurface = c_style_suface;
	}


} // namespace Helios::Vulkan
