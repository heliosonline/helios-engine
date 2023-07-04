#include "pch.h"

#include "Platform/Renderer/Vulkan/Core/Devices.h"
#include "HeliosEngine/Renderer/Renderer.h"

#include "Platform/Renderer/Vulkan/Core/Instance.h"

#include <GLFW/glfw3.h>


namespace Helios::Vulkan {


	Devices::Devices()
	{
		m_Instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();
	}


	void Devices::Create()
	{
		// Setup list of layers
#		ifdef BUILD_DEBUG
			m_ListLayers.push_back("VK_LAYER_KHRONOS_validation");
#		endif

		// Setup list of extensions
		m_ListExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		// Pick best suitable physical device
		LOG_RENDER_DEBUG("Choosing vulkan physical device...");
		PickPhysicalDevice();

		// Create the logical device
		LOG_RENDER_DEBUG("Creating vulkan logical device...");
		CreateLogicalDevice();
		GetQueues();
	}


	void Devices::Destroy()
	{
		if (m_vkLogicalDevice)
			m_vkLogicalDevice.destroy();
	}


	void Devices::PickPhysicalDevice()
	{
		// Get all suitable devices
		GetPhysicalDevices();

		// Pick the most suitable device
		PhysicalDeviceInfo current = {};
		for (auto entry : m_ListPhysicalDevices)
		{
			if (current.score < entry.score)
			{
				m_vkPhysicalDevice = entry.device;
				current = entry;
			}
		}

		if (!m_vkPhysicalDevice)
		{
			LOG_RENDER_FATAL("Failed to find a suitable physical device!");
			return;
		}

		LOG_RENDER_INFO("Selected physical device: ({:04X}:{:04X}) \"{}\"",
			current.vendorID, current.deviceID, current.name);
	}


	std::vector<PhysicalDeviceInfo>& Devices::GetPhysicalDevices()
    {
		static bool s_aqquired = false;

		// Return saved list
		if (s_aqquired)
			return m_ListPhysicalDevices;

		// Get all devices
		std::vector<vk::PhysicalDevice> available = m_Instance->Get().enumeratePhysicalDevices();
		for (auto dev : available)
		{
			// Get device properties
			vk::PhysicalDeviceProperties props = dev.getProperties();

			// Log device
			LOG_RENDER_INFO("Physical device:");
			LOG_RENDER_INFO("[ INFO ] Device: ({:04X}:{:04X}) \"{}\"",
				props.vendorID, props.deviceID,
				props.deviceName);
			switch (props.deviceType)
			{
			case vk::PhysicalDeviceType::eDiscreteGpu:   LOG_RENDER_DEBUG("[ INFO ] Type: Discrete GPU");   break;
			case vk::PhysicalDeviceType::eIntegratedGpu: LOG_RENDER_DEBUG("[ INFO ] Type: Integrated GPU"); break;
			case vk::PhysicalDeviceType::eVirtualGpu:    LOG_RENDER_DEBUG("[ INFO ] Type: Virtual GPU");    break;
			case vk::PhysicalDeviceType::eCpu:           LOG_RENDER_DEBUG("[ INFO ] Type: CPU");            break;
			case vk::PhysicalDeviceType::eOther:         LOG_RENDER_DEBUG("[ INFO ] Type: Other");          break;
			}
			LOG_RENDER_DEBUG("[ INFO ] API v{}.{}.{}.{}",
				VK_API_VERSION_VARIANT(props.apiVersion),
				VK_API_VERSION_MAJOR(props.apiVersion),
				VK_API_VERSION_MINOR(props.apiVersion),
				VK_API_VERSION_PATCH(props.apiVersion));

			// Check extensions
			std::set<std::string> required(m_ListExtensions.begin(), m_ListExtensions.end());
			if (LOG_LEVEL < LOG_LEVEL_DEBUG)
				LOG_RENDER_TRACE("Supported device extensions ({}):", dev.enumerateDeviceExtensionProperties().size());
			else
				LOG_RENDER_DEBUG("Required device extensions ({}/{}):", required.size(), dev.enumerateDeviceExtensionProperties().size());
			for (auto& ext : dev.enumerateDeviceExtensionProperties())
			{
				if (required.contains(ext.extensionName))
					LOG_RENDER_DEBUG("[  OK  ] \"{}\"", ext.extensionName);
				else
					LOG_RENDER_TRACE("[UNUSED] \"{}\"", ext.extensionName);
				required.erase(ext.extensionName);
			}
			if (!required.empty())
			{
				LOG_RENDER_DEBUG("Unsupported device extensions ({}):", required.size());
				for (auto req : required)
					LOG_RENDER_DEBUG("[FAILED] \"{}\"", req);
				continue;
			}

			// Find suitable queue families
			QueueFamilyIndices indices = GetQueueFamilies(dev);
			if (!indices.complete())
			{
				LOG_RENDER_DEBUG("Unsupported queue families:");
				if (!indices.graphicsFamily.has_value())
					LOG_RENDER_DEBUG("[FAILED] Graphics Queue");
				if (!indices.presentFamily.has_value())
					LOG_RENDER_DEBUG("[FAILED] Presentation Queue");
				continue;
			}


			// Setup device entry
			PhysicalDeviceInfo newEntry;
			newEntry.device = dev;
			newEntry.name = props.deviceName.data();
			newEntry.deviceID = props.deviceID;
			newEntry.vendorID = props.vendorID;
			newEntry.type = props.deviceType;
			newEntry.score = RateSuitability(dev);
			LOG_RENDER_INFO("[ INFO ] Device is suitable (score: {}).", newEntry.score);

			// Add device entry to the list
			m_ListPhysicalDevices.push_back(newEntry);
		}

		// Return the list
		return m_ListPhysicalDevices;
	}


	int Devices::RateSuitability(const vk::PhysicalDevice& device)
	{
		int score = 0;

		// Get device properties
		vk::PhysicalDeviceProperties props = device.getProperties();

		// Score based on type
		switch (props.deviceType)
		{
		case vk::PhysicalDeviceType::eDiscreteGpu:   score = 1000; break;
		case vk::PhysicalDeviceType::eIntegratedGpu: score =  500; break;
		case vk::PhysicalDeviceType::eVirtualGpu:    score =  200; break;
		case vk::PhysicalDeviceType::eCpu:           score =  100; break;
		case vk::PhysicalDeviceType::eOther:         score =    0; break;
		}

		// TODO...
		// TODO...
		// TODO...

		return score;
	}


	QueueFamilyIndices Devices::GetQueueFamilies(vk::PhysicalDevice device)
	{
		if (!device)
			device = m_vkPhysicalDevice;

		QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		uint32_t i = 0;
		for (auto& q : queueFamilies)
		{
			// Check graphics support
			if (q.queueFlags & vk::QueueFlagBits::eGraphics)
				indices.graphicsFamily = i;

			// Check presentation support (both: GLFW, native)
			if (glfwGetPhysicalDevicePresentationSupport(m_Instance->Get(), device, i) == GLFW_TRUE)
			{
				if (device.getSurfaceSupportKHR(i, m_Instance->GetSurface()))
					indices.presentFamily = i;
			}

			// Done?
			if (indices.complete())
				break;

			// Next
			i++;
		}

		return indices;
	}


	void Devices::CreateLogicalDevice()
	{
		// Setup QueueInfo
		QueueFamilyIndices indices = GetQueueFamilies(m_vkPhysicalDevice);
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
				q.queueFamilyIndex = index;
				q.queueCount = 1;
				q.pQueuePriorities = &queuePriority;
			}
			QueueInfo.push_back(q);
		}

		// Setup features
		vk::PhysicalDeviceFeatures DeviceFeatures = vk::PhysicalDeviceFeatures();
		{
			//DeviceFeatures.setXYZ...
		}

		// Setup DeviceInfo
		vk::DeviceCreateInfo DeviceInfo = vk::DeviceCreateInfo();
		{
			DeviceInfo.queueCreateInfoCount = (uint32_t)QueueInfo.size();
			DeviceInfo.pQueueCreateInfos = QueueInfo.data();
			DeviceInfo.enabledLayerCount = (uint32_t)m_ListLayers.size();
			DeviceInfo.ppEnabledLayerNames = m_ListLayers.data();
			DeviceInfo.enabledExtensionCount = (uint32_t)m_ListExtensions.size();
			DeviceInfo.ppEnabledExtensionNames = m_ListExtensions.data();
			DeviceInfo.pEnabledFeatures = &DeviceFeatures;
		}

		// Create the logical device
		try {
			m_vkLogicalDevice = m_vkPhysicalDevice.createDevice(DeviceInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_FATAL("Failed to create logical device!");
		}
	}


	void Devices::GetQueues()
	{
		QueueFamilyIndices indices = GetQueueFamilies(m_vkPhysicalDevice);
		m_vkGraphicsQueue = m_vkLogicalDevice.getQueue(indices.graphicsFamily.value(), 0);
		m_vkPresentQueue = m_vkLogicalDevice.getQueue(indices.presentFamily.value(), 0);
	}


} // namespace Helios::Vulkan
