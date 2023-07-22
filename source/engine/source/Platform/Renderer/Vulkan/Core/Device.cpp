#include "pch.h"
#include "Device.h"

#include "HeliosEngine/Renderer/Renderer.h"
#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include <GLFW/glfw3.h>


namespace Helios::Vulkan {


	Device::Device()
	{
		Create();
	}


	Device::~Device()
	{
		Destroy();
	}


	void Device::Create()
	{
		LOG_RENDER_TRACE("Creating device objects...");

		PickPhysicalDevice();
		CreateLogicalDevice();
		GetQueues();
		CreateCommandPool();
	}


	void Device::Destroy()
	{
		LOG_RENDER_TRACE("Destroying device objects...");

		if (m_vkCommandPool)
			m_vkLogicalDevice.destroyCommandPool(m_vkCommandPool);
		if (m_vkLogicalDevice)
			m_vkLogicalDevice.destroy();
	}


	QueueFamilyIndices Device::QueryQueueFamilies(vk::PhysicalDevice device)
	{
		Scope<Instance> &instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();

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
			if (glfwGetPhysicalDevicePresentationSupport(instance->GetInstance(), device, i) == GLFW_TRUE)
			{
				if (device.getSurfaceSupportKHR(i, instance->GetSurface()))
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


	vk::Format Device::QuerySupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
	{
		for (vk::Format format : candidates)
		{
			vk::FormatProperties props = m_vkPhysicalDevice.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
				return format;
		}
		LOG_RENDER_EXCEPT("Failed to find supported format!");
	}


	uint32_t Device::QueryMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags props)
	{
		vk::PhysicalDeviceMemoryProperties memProps = m_vkPhysicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props)
				return i;
		}

		LOG_RENDER_EXCEPT("Failed to query suitable memory type!");
	}


	void Device::CreateImageWithMemory(const vk::ImageCreateInfo &imageInfo, vk::MemoryPropertyFlags props, vk::Image &image, vk::DeviceMemory &imageMemory)
	{
		try {
			image = m_vkLogicalDevice.createImage(imageInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create image!");
		}

		vk::MemoryRequirements memReq = m_vkLogicalDevice.getImageMemoryRequirements(image);
		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo();
		{
			allocInfo.allocationSize = memReq.size;
			allocInfo.memoryTypeIndex = QueryMemoryType(memReq.memoryTypeBits, props);
		}

		try {
			imageMemory = m_vkLogicalDevice.allocateMemory(allocInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to allocate image memory!");
		}

		try {
			m_vkLogicalDevice.bindImageMemory(image, imageMemory, 0);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to bind image memory!")
		}
	}


	void Device::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
	{
		vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo();
		{
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = vk::SharingMode::eExclusive;
		}

		if (m_vkLogicalDevice.createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) {
			LOG_RENDER_EXCEPT("Failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements;
		m_vkLogicalDevice.getBufferMemoryRequirements(buffer, &memRequirements);

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo();
		{
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = QueryMemoryType(memRequirements.memoryTypeBits, props);
		}

		if (m_vkLogicalDevice.allocateMemory(&allocInfo, nullptr, &bufferMemory) != vk::Result::eSuccess) {
			LOG_RENDER_EXCEPT("Failed to allocate vertex buffer memory!");
		}

		m_vkLogicalDevice.bindBufferMemory(buffer, bufferMemory, 0);
	}


	std::vector<const char*> Device::GetRequiredLayers()
	{
		std::vector<const char*> layers;

		// Setup list of layers
#		ifdef BUILD_DEBUG
			layers.push_back("VK_LAYER_KHRONOS_validation");
#		endif

		return layers;
	}


	std::vector<const char*> Device::GetRequiredExtensions()
	{
		std::vector<const char*> extensions;

		// Setup list of extensions
		extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		return extensions;
	}


	void Device::PickPhysicalDevice()
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
			LOG_RENDER_EXCEPT("Failed to find a suitable physical device!");

		LOG_RENDER_INFO("Selected physical device: ({:04X}:{:04X}) \"{}\"",
			current.vendorID, current.deviceID, current.name);
	}


	std::vector<PhysicalDeviceInfo>& Device::GetPhysicalDevices()
	{
		Scope<Instance> &instance = static_cast<VKRendererAPI*>(Renderer::Get())->GetInstance();
		static bool s_aqquired = false;

		// Return saved list
		if (s_aqquired)
			return m_ListPhysicalDevices;

		// Get all devices
		std::vector<vk::PhysicalDevice> available = instance->GetInstance().enumeratePhysicalDevices();
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
			auto extensions = GetRequiredExtensions();
			std::set<std::string> required(extensions.begin(), extensions.end());
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
			QueueFamilyIndices indices = QueryQueueFamilies(dev);
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


	int Device::RateSuitability(const vk::PhysicalDevice &device)
	{
		int score = 0;

		// Get device properties
		vk::PhysicalDeviceProperties props = device.getProperties();

		// Score based on type
		switch (props.deviceType)
		{
		case vk::PhysicalDeviceType::eDiscreteGpu:   score = 1000; break;
		case vk::PhysicalDeviceType::eIntegratedGpu: score = 500; break;
		case vk::PhysicalDeviceType::eVirtualGpu:    score = 200; break;
		case vk::PhysicalDeviceType::eCpu:           score = 100; break;
		case vk::PhysicalDeviceType::eOther:         score = 0; break;
		}

		// 128 bytes garanteed
		// 256 bytes more common
		if (props.limits.maxPushConstantsSize >= 256)
			score += 100;

		// TODO...
		// TODO...
		// TODO...

		return score;
	}


	void Device::CreateLogicalDevice()
	{
		// Setup QueueInfo
		QueueFamilyIndices indices = QueryQueueFamilies(m_vkPhysicalDevice);
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
			DeviceFeatures.setSamplerAnisotropy(VK_TRUE);
		}

		// Setup DeviceInfo
		auto layers = GetRequiredLayers();
		auto extensions = GetRequiredExtensions();
		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo();
		{
			deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueInfo.size());
			deviceInfo.pQueueCreateInfos = QueueInfo.data();
			deviceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			deviceInfo.ppEnabledLayerNames = layers.data();
			deviceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			deviceInfo.ppEnabledExtensionNames = extensions.data();
			deviceInfo.pEnabledFeatures = &DeviceFeatures;
		}

		// Create the logical device
		try {
			LOG_RENDER_TRACE("Creating logical device...");
			m_vkLogicalDevice = m_vkPhysicalDevice.createDevice(deviceInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create logical device!");
		}
	}


	void Device::GetQueues()
	{
		QueueFamilyIndices indices = QueryQueueFamilies(m_vkPhysicalDevice);
		m_vkGraphicsQueue = m_vkLogicalDevice.getQueue(indices.graphicsFamily.value(), 0);
		m_vkPresentQueue = m_vkLogicalDevice.getQueue(indices.presentFamily.value(), 0);
	}


	void Device::CreateCommandPool()
	{
		QueueFamilyIndices indices = QueryQueueFamilies(m_vkPhysicalDevice);
		vk::CommandPoolCreateInfo poolInfo = vk::CommandPoolCreateInfo();
		{
			poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
			poolInfo.flags =
				vk::CommandPoolCreateFlagBits::eTransient |
				vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		}

		try {
			LOG_RENDER_TRACE("Creating command pool...");
			m_vkCommandPool = m_vkLogicalDevice.createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_EXCEPT("Failed to create command pool!");
		}
	}


} // namespace Helios::Vulkan
