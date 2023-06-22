#include "pch.h"

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include "HeliosEngine/Core/Application.h"
#include "HeliosEngine/Core/Config.h"


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
		CreateSurface();

		ChoosePhysicalDevice();
		CreateLogicalDevice();
		GetQueues();
	}


	void VKRendererAPI::Shutdown()
	{
		LOG_RENDER_DEBUG("Shutting down vulkan renderer.");

		// Device objects
		if (m_vkDevice)
			m_vkDevice.destroy();

		// Instance objects
		if (m_vkSurface)
			m_vkInstance.destroySurfaceKHR(m_vkSurface);
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
		LOG_RENDER_ASSERT(CheckSupportedInstanceLayers(layers), "Required layers not supported!");

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
		LOG_RENDER_ASSERT(CheckSupportedInstanceExtensions(extensions), "Required extensions not supported!");

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
			LOG_RENDER_ASSERT(0, "Failed to create instance!");
		}

		// Get the function loader
		m_vkLoader = vk::DispatchLoaderDynamic(m_vkInstance, vkGetInstanceProcAddr);

		// Create debug callback
		#ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("Creating debug messenger.");
			CreateDebugMessanger();
		#endif
	}


	void VKRendererAPI::CreateDebugMessanger()
	{
		vk::DebugUtilsMessengerCreateInfoEXT vkCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT();
		{
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
		}

		m_vkDebugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(vkCreateInfo, nullptr, m_vkLoader);
		LOG_RENDER_ASSERT(m_vkDebugMessenger, "Failed to create debug messenger!");
	}


	bool VKRendererAPI::CheckSupportedInstanceLayers(const std::vector<const char*>& layers)
	{
		std::vector<vk::LayerProperties> vkSupported = vk::enumerateInstanceLayerProperties();

		#if (LOG_LEVEL <= LOG_LEVEL_TRACE)
			LOG_RENDER_TRACE("Supported instance layers:");
			for (auto l : vkSupported)
				LOG_RENDER_TRACE("- \"{}\" v{}.{}.{}.{}", l.layerName,
					VK_API_VERSION_VARIANT(l.specVersion),
					VK_API_VERSION_MAJOR(l.specVersion),
					VK_API_VERSION_MINOR(l.specVersion),
					VK_API_VERSION_PATCH(l.specVersion));
		#endif

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


	bool VKRendererAPI::CheckSupportedInstanceExtensions(const std::vector<const char*>& extensions)
	{
		std::vector<vk::ExtensionProperties> vkSupported = vk::enumerateInstanceExtensionProperties();

		#if (LOG_LEVEL <= LOG_LEVEL_TRACE)
			LOG_RENDER_TRACE("Supported instance extensions:");
			for (auto e : vkSupported)
				LOG_RENDER_TRACE("- \"{}\" v{}.{}.{}.{}", e.extensionName,
					VK_API_VERSION_VARIANT(e.specVersion),
					VK_API_VERSION_MAJOR(e.specVersion),
					VK_API_VERSION_MINOR(e.specVersion),
					VK_API_VERSION_PATCH(e.specVersion));
		#endif

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


	void VKRendererAPI::CreateSurface()
	{
		VkSurfaceKHR c_style_suface;
		if (glfwCreateWindowSurface(m_vkInstance, (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &c_style_suface) != VK_SUCCESS)
			LOG_RENDER_ASSERT(0, "Failed to create surface!");

		m_vkSurface = c_style_suface;
	}


	void VKRendererAPI::ChoosePhysicalDevice()
	{
		// Get all devices
		std::vector<vk::PhysicalDevice> available = m_vkInstance.enumeratePhysicalDevices();

		// Try previous selection
		if (Config::Get("VulkanPhysDevName").length() and Config::Get("VulkanPhysDevID").length())
		{
			LOG_RENDER_TRACE("Previous physical device:");
			LOG_RENDER_TRACE("- Name: \"{}\"", Config::Get("VulkanPhysDevName"));
			LOG_RENDER_TRACE("- ID: {}", Config::Get("VulkanPhysDevID"));
			for (auto d : available)
			{
				vk::PhysicalDeviceProperties props = d.getProperties();
				if ((Config::Get("VulkanPhysDevName").compare(props.deviceName.data()) == 0) and
					(Config::Get("VulkanPhysDevID").compare(std::to_string(props.deviceID)) == 0) and
					IsPhysicalDeviceSuitable(d))
				{
					LOG_RENDER_INFO("Previous device selected.");
					m_vkPhysDevice = d;
					return;
				}
			}
		}

		// Find new selection
		for (auto d : available)
		{
			// Get device properties
			vk::PhysicalDeviceProperties props = d.getProperties();

			// Log device
			#if (LOG_LEVEL <= LOG_LEVEL_TRACE)
				LOG_RENDER_TRACE("Physical device:");
				LOG_RENDER_TRACE("- Name: \"{}\"", props.deviceName);
				switch (props.deviceType)
				{
				case vk::PhysicalDeviceType::eCpu:           LOG_RENDER_TRACE("- Type: CPU");            break;
				case vk::PhysicalDeviceType::eDiscreteGpu:   LOG_RENDER_TRACE("- Type: Discrete GPU");   break;
				case vk::PhysicalDeviceType::eIntegratedGpu: LOG_RENDER_TRACE("- Type: Integrated GPU"); break;
				case vk::PhysicalDeviceType::eVirtualGpu:    LOG_RENDER_TRACE("- Type: Virtual GPU");    break;
				case vk::PhysicalDeviceType::eOther:         LOG_RENDER_TRACE("- Type: Other");          break;
				}
				LOG_RENDER_TRACE("- API v{}.{}.{}.{}",
					VK_API_VERSION_VARIANT(props.apiVersion),
					VK_API_VERSION_MAJOR(props.apiVersion),
					VK_API_VERSION_MINOR(props.apiVersion),
					VK_API_VERSION_PATCH(props.apiVersion));
				LOG_RENDER_TRACE("- ID: {}", props.deviceID);
			#endif

			// Select device if suitable
			if (IsPhysicalDeviceSuitable(d))
			{
				// Save selection
				Config::Set("VulkanPhysDevName", props.deviceName);
				Config::Set("VulkanPhysDevID", std::to_string(props.deviceID));

				m_vkPhysDevice = d;
				return;
			}
		}

		LOG_RENDER_ASSERT(m_vkPhysDevice, "Failed to choose physical device!");
	}


	bool VKRendererAPI::IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device)
	{
		// Setup list of required extensions
		const std::vector<const char*> requested = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Check if extensions are supported
		if (!CheckSupportedDeviceExtensions(device, requested))
			return false;

		// Find suitable queue families
		QueueFamilyIndices indices = ChooseQueueFamilies(device);
		if (!indices.complete())
			return false;

		return true;
	}


	bool VKRendererAPI::CheckSupportedDeviceExtensions(const vk::PhysicalDevice& device, const std::vector<const char*>& requested)
	{
		std::set<std::string> required(requested.begin(), requested.end());

		LOG_RENDER_TRACE("Supported device extensions:");
		for (auto& e : device.enumerateDeviceExtensionProperties())
		{
			LOG_RENDER_TRACE("- \"{}\" v{}.{}.{}.{}", e.extensionName,
				VK_API_VERSION_VARIANT(e.specVersion),
				VK_API_VERSION_MAJOR(e.specVersion),
				VK_API_VERSION_MINOR(e.specVersion),
				VK_API_VERSION_PATCH(e.specVersion));
			required.erase(e.extensionName);
		}

		return required.empty();
	}


	QueueFamilyIndices VKRendererAPI::ChooseQueueFamilies(const vk::PhysicalDevice& device)
	{
		QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		uint32_t i = 0;
		for (auto& q : queueFamilies)
		{
			if (q.queueFlags & vk::QueueFlagBits::eGraphics)
				indices.graphicsFamily = i;
//			if (glfwGetPhysicalDevicePresentationSupport(m_vkInstance, device, i) == GLFW_TRUE)
			if (device.getSurfaceSupportKHR(i, m_vkSurface))
				indices.presentFamily = i;

			if (indices.complete())
				break;

			i++;
		}

		return indices;
	}


	void VKRendererAPI::CreateLogicalDevice()
	{
		// Setup queue
		QueueFamilyIndices indices = ChooseQueueFamilies(m_vkPhysDevice);
		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(indices.graphicsFamily.value());
		if (indices.graphicsFamily.value() != indices.presentFamily.value())
			uniqueIndices.push_back(indices.presentFamily.value());
		float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> QueueInfo;
		for (uint32_t index : uniqueIndices)
		{
			vk::DeviceQueueCreateInfo q = vk::DeviceQueueCreateInfo();
			{
				q.setQueueFamilyIndex(index);
				q.setQueueCount(1);
				q.setPQueuePriorities(&queuePriority);
			}
			QueueInfo.push_back(q);
		}

		// Setup layers
		std::vector<const char*> layers;
		#ifdef BUILD_DEBUG
			LOG_RENDER_DEBUG("Using the \"VK_LAYER_KHRONOS_validation\" layer.");
			layers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		// Setup extensions
		std::vector<const char*> extensions;

		// Setup features
		vk::PhysicalDeviceFeatures DeviceFeatures = vk::PhysicalDeviceFeatures();
		{
			//DeviceFeatures.setXYZ...
		}

		// Setup DeviceInfo
		vk::DeviceCreateInfo DeviceInfo = vk::DeviceCreateInfo();
		{
			// Queue
			DeviceInfo.setQueueCreateInfoCount((uint32_t)QueueInfo.size());
			DeviceInfo.setPQueueCreateInfos(QueueInfo.data());
			// Layer
			DeviceInfo.setEnabledLayerCount((uint32_t)layers.size());
			DeviceInfo.setPpEnabledLayerNames(layers.data());
			// Extensions
			DeviceInfo.setEnabledExtensionCount((uint32_t)extensions.size());
			DeviceInfo.setPpEnabledExtensionNames(extensions.data());
			// Features
			DeviceInfo.setPEnabledFeatures(&DeviceFeatures);
		}

		// Create the logical device
		try {
			m_vkDevice = m_vkPhysDevice.createDevice(DeviceInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_ASSERT(0, "Failed to create logical device!");
		}
	}


	void VKRendererAPI::GetQueues()
	{
		QueueFamilyIndices indices = ChooseQueueFamilies(m_vkPhysDevice);

		m_vkGraphicsQueue = m_vkDevice.getQueue(indices.graphicsFamily.value(), 0);
		m_vkPresentQueue = m_vkDevice.getQueue(indices.presentFamily.value(), 0);
	}


} // namespace Helios
