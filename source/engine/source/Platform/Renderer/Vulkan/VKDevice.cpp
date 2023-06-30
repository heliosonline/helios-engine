#include "pch.h"

#include "Platform/Renderer/Vulkan/VKDevice.h"

#include <GLFW/glfw3.h>


namespace Helios {


	VKDevice::VKDevice(Ref<VKInstance>& inst)
		: m_Instance(inst)
	{
		// Setup list of layers
		#ifdef BUILD_DEBUG
			m_ListLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		// Setup list of extensions
		m_ListExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}


	void VKDevice::Create()
	{
		// Pick best suitable physical device
		PickPhysicalDevice();

		// Create the logical device
		CreateLogicalDevice();
		GetQueues();
	}


	void VKDevice::Destroy()
	{
		if (m_vkLogicalDevice)
			m_vkLogicalDevice.destroy();
	}


	void VKDevice::PickPhysicalDevice()
	{
		// Get all suitable devices
		GetPhysicalDevices();

		// Pick the most suitable device
		PhysicalDeviceInfo current;
		for (auto entry : m_ListPhysicalDevices)
		{
			if (current.score < entry.score)
			{
				m_vkPhysicalDevice = entry.device;
				current = entry;
			}
		}

		LOG_RENDER_ASSERT(m_vkPhysicalDevice, "Failed to find a suitable physical device!");

		LOG_RENDER_DEBUG("Selected physical device: ({:04X}:{:04X}) \"{}\"",
			current.vendorID, current.deviceID, current.name);
	}


	std::vector<PhysicalDeviceInfo>& VKDevice::GetPhysicalDevices()
    {
		static bool s_aqquired = false;

		// Return saved list
		if (s_aqquired)
			return m_ListPhysicalDevices;

		// Get all devices
		std::vector<vk::PhysicalDevice> available = m_Instance->GetInstance().enumeratePhysicalDevices();
		for (auto d : available)
		{
			// Get device properties
			vk::PhysicalDeviceProperties props = d.getProperties();

			// Log device
			#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
				LOG_RENDER_DEBUG("Physical device:");
				LOG_RENDER_DEBUG("[ INFO ] Device: ({:04X}:{:04X}) \"{}\"",
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
			#endif

			// Check extensions
			std::set<std::string> required(m_ListExtensions.begin(), m_ListExtensions.end());
			if (LOG_LEVEL < LOG_LEVEL_DEBUG)
				LOG_RENDER_TRACE("Supported device extensions ({}):", d.enumerateDeviceExtensionProperties().size());
			else
				LOG_RENDER_DEBUG("Required device extensions ({}/{}):", required.size(), d.enumerateDeviceExtensionProperties().size());
			for (auto& e : d.enumerateDeviceExtensionProperties())
			{
				if (required.contains(e.extensionName))
					LOG_RENDER_DEBUG("[  OK  ] \"{}\"", e.extensionName);
				else
					LOG_RENDER_TRACE("[UNUSED] \"{}\"", e.extensionName);
				required.erase(e.extensionName);
			}
			if (!required.empty())
			{
				LOG_RENDER_DEBUG("Unsupported device extensions ({}):", required.size());
				for (auto req : required)
					LOG_RENDER_DEBUG("[FAILED] \"{}\"", req);
				continue;
			}

			// Find suitable queue families
			QueueFamilyIndices indices = FindQueueFamilies(d);
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
			newEntry.device = d;
			newEntry.name = props.deviceName.data();
			newEntry.deviceID = props.deviceID;
			newEntry.vendorID = props.vendorID;
			newEntry.type = props.deviceType;
			newEntry.score = RateSuitability(d);
			LOG_RENDER_DEBUG("[ INFO ] Device is suitable (score: {}).", newEntry.score);

			// Add device entry to the list
			m_ListPhysicalDevices.push_back(newEntry);
		}

		// Return the list
		return m_ListPhysicalDevices;
	}


	int VKDevice::RateSuitability(const vk::PhysicalDevice& device)
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


	QueueFamilyIndices VKDevice::FindQueueFamilies(const vk::PhysicalDevice& device)
	{
		QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		uint32_t i = 0;
		for (auto& q : queueFamilies)
		{
			// Check graphics support
			if (q.queueFlags & vk::QueueFlagBits::eGraphics)
				indices.graphicsFamily = i;

			// Check presentation support (both: GLFW, native)
			if (glfwGetPhysicalDevicePresentationSupport(m_Instance->GetInstance(), device, i) == GLFW_TRUE)
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


	void VKDevice::CreateLogicalDevice()
	{
		// Setup QueueInfo
		QueueFamilyIndices indices = FindQueueFamilies(m_vkPhysicalDevice);
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
			DeviceInfo.setEnabledLayerCount((uint32_t)m_ListLayers.size());
			DeviceInfo.setPpEnabledLayerNames(m_ListLayers.data());
			// Extensions
			DeviceInfo.setEnabledExtensionCount((uint32_t)m_ListExtensions.size());
			DeviceInfo.setPpEnabledExtensionNames(m_ListExtensions.data());
			// Features
			DeviceInfo.setPEnabledFeatures(&DeviceFeatures);
		}

		// Create the logical device
		try {
			m_vkLogicalDevice = m_vkPhysicalDevice.createDevice(DeviceInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_ASSERT(0, "Failed to create logical device!");
		}
	}


	void VKDevice::GetQueues()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_vkPhysicalDevice);
		m_vkGraphicsQueue = m_vkLogicalDevice.getQueue(indices.graphicsFamily.value(), 0);
		m_vkPresentQueue = m_vkLogicalDevice.getQueue(indices.presentFamily.value(), 0);
	}


} // namespace Helios
