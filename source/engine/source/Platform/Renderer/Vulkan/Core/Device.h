#pragma once

#include <vulkan/vulkan.hpp>

namespace Helios::Vulkan {


	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};


	struct PhysicalDeviceInfo
	{
		vk::PhysicalDevice device = {};
		std::string name;
		uint32_t vendorID;
		uint32_t deviceID;
		vk::PhysicalDeviceType type;
		int score;
	};


	class Device
	{
	public:
		Device();
		~Device();

		void Create();
		void Destroy();

	// Getter for vulkan objects
	public:
		vk::PhysicalDevice& GetPhysicalDevice() { return m_vkPhysicalDevice; }
		vk::Device& GetLogicalDevice() { return m_vkLogicalDevice; }
		vk::Queue& GetGraphicsQueue() { return m_vkGraphicsQueue; }
		vk::Queue& GetPresentQueue() { return m_vkPresentQueue; }
		vk::CommandPool& GetCommandPool() { return m_vkCommandPool; }

		QueueFamilyIndices QueryQueueFamilies(vk::PhysicalDevice device = nullptr);
		vk::Format QuerySupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
		uint32_t QueryMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags props);

		void CreateImageWithMemory(const vk::ImageCreateInfo &imageInfo, vk::MemoryPropertyFlags props, vk::Image &image, vk::DeviceMemory &imageMemory);
		void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);

	// Vulkan objects
	private:
		vk::PhysicalDevice m_vkPhysicalDevice;
		vk::Device m_vkLogicalDevice;
		vk::Queue m_vkGraphicsQueue;
		vk::Queue m_vkPresentQueue;
		vk::CommandPool m_vkCommandPool;

	// Internal helper
	private:
		std::vector<const char*> GetRequiredLayers();
		std::vector<const char*> GetRequiredExtensions();
		void PickPhysicalDevice();
		std::vector<PhysicalDeviceInfo>& GetPhysicalDevices();
		int RateSuitability(const vk::PhysicalDevice &device);
		void CreateLogicalDevice();
		void GetQueues();
		void CreateCommandPool();

	// Internal data
	private:
		std::vector<PhysicalDeviceInfo> m_ListPhysicalDevices;
	};


} // namespace Helios::Vulkan
