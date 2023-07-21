#include "pch.h"

#include "Platform/Renderer/Vulkan/Core/Instance.h"

#include "HeliosEngine/Core/Application.h"


namespace Helios::Vulkan {


	VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
		vk::DebugUtilsMessageSeverityFlagsEXT severity,
		vk::DebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT

		if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
			LOG_RENDER_ERROR(pCallbackData->pMessage);
		if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
			LOG_RENDER_WARN(pCallbackData->pMessage);
		if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
			LOG_RENDER_INFO(pCallbackData->pMessage);
		if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
			LOG_RENDER_TRACE(pCallbackData->pMessage);

		return VK_FALSE;
	}


	Instance::Instance()
	{
		Create();
	}


	Instance::~Instance()
	{
		Destroy();
	}


	void Instance::Create()
	{
		LOG_RENDER_TRACE("Creating instance objects...");

		// Get vulkan versions
		uint32_t vkVer;
		vkEnumerateInstanceVersion(&vkVer);
		LOG_RENDER_DEBUG("Systems max supported vulkan version: {}.{}.{}.{}",
			VK_API_VERSION_VARIANT(vkVer),
			VK_API_VERSION_MAJOR(vkVer),
			VK_API_VERSION_MINOR(vkVer),
			VK_API_VERSION_PATCH(vkVer));
		LOG_RENDER_DEBUG("Engines max supported vulkan version: {}.{}.{}.{}",
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

		// Check layer support
		if (!CheckSupportedLayers())
			LOG_RENDER_EXCEPT("Required instance layers not supported!");

		// Check extension support
		if (!CheckSupportedExtensions())
			LOG_RENDER_EXCEPT("Required instance extensions not supported!");

		// Setup InstanceInfo
		auto layers = GetRequiredLayers();
		auto extensions = GetRequiredExtensions();
		vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo();
		{
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			instanceInfo.ppEnabledLayerNames = layers.data();
			instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			instanceInfo.ppEnabledExtensionNames = extensions.data();
		}

		// Create the vulkan instance
		try {
			LOG_RENDER_TRACE("Creating instance...");
			m_vkInstance = vk::createInstance(instanceInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create instance!");
		}

		// Get the function loader
		m_vkLoader = vk::DispatchLoaderDynamic(m_vkInstance, vkGetInstanceProcAddr);

		// Create debug callback
#		ifdef BUILD_DEBUG
			CreateDebugMessenger();
#		endif

		// Create the surface
		CreateSurface();
	}


	void Instance::Destroy()
	{
		LOG_RENDER_TRACE("Destroying instance objects...");

		if (m_vkSurface)
			m_vkInstance.destroySurfaceKHR(m_vkSurface);

		if (m_vkDebugMessenger)
			m_vkInstance.destroyDebugUtilsMessengerEXT(m_vkDebugMessenger, nullptr, m_vkLoader);

		if (m_vkInstance)
			m_vkInstance.destroy();
	}


	std::vector<const char*> Instance::GetRequiredLayers()
	{
		std::vector<const char*> layers;

		// Setup list of layers
#		ifdef BUILD_DEBUG
			layers.push_back("VK_LAYER_KHRONOS_validation");
#		endif

		return layers;
	}


	std::vector<const char*> Instance::GetRequiredExtensions()
	{
		// Get required extensions for GLFW
		uint32_t glfwExtCount = 0;
		const char** glfwExt;
		glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);
		std::vector<const char*> extensions = std::vector<const char*>(glfwExt, glfwExt + glfwExtCount);

		// Setup list of extensions
#		ifdef BUILD_DEBUG
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#		endif

		return extensions;
	}


	bool Instance::CheckSupportedLayers()
	{
		auto layers = GetRequiredLayers();
		std::vector<vk::LayerProperties> supported = vk::enumerateInstanceLayerProperties();
		std::set<std::string> required(layers.begin(), layers.end());

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
		auto extensions = GetRequiredExtensions();
		std::vector<vk::ExtensionProperties> supported = vk::enumerateInstanceExtensionProperties();
		std::set<std::string> required(extensions.begin(), extensions.end());

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


	void Instance::CreateDebugMessenger()
	{
		LOG_RENDER_TRACE("Creating debug messenger...");

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
		if (!m_vkDebugMessenger)
			LOG_RENDER_EXCEPT("Failed to create debug messenger!");
	}


	void Instance::CreateSurface()
	{
		LOG_RENDER_TRACE("Creating surface...");

		VkSurfaceKHR c_style_suface;
		if (glfwCreateWindowSurface(m_vkInstance, (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &c_style_suface) != VK_SUCCESS)
			LOG_RENDER_EXCEPT("Failed to create surface!");

		m_vkSurface = c_style_suface;
	}


} // namespace Helios::Vulkan
