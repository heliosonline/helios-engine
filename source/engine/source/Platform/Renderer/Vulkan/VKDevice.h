#pragma once

#include <vulkan/vulkan.hpp>

#include "Platform/Renderer/Vulkan/VKInstance.h"


namespace Helios {

	struct PhysicalDeviceInfo
	{
		vk::PhysicalDevice device = {};
//		std::array<char, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE> name;
		std::string name;
		uint32_t vendorID;
		uint32_t deviceID;
		vk::PhysicalDeviceType type;
		int score;
	};


	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};


	class VKDevice
	{
	public:
		VKDevice(Ref<VKInstance>& inst);

		void Create();
		void Destroy();

		vk::PhysicalDevice& GetPhysicalDevice() { return m_vkPhysicalDevice; }
		vk::Device& GetLogicalDevice() { return m_vkLogicalDevice; }

		QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device);

	private:
		void PickPhysicalDevice();
		std::vector<PhysicalDeviceInfo>& GetPhysicalDevices();
		int RateSuitability(const vk::PhysicalDevice& device);
		void CreateLogicalDevice();
		void GetQueues();

	private:
		// Native vulkan objects
		vk::PhysicalDevice m_vkPhysicalDevice;
		vk::Device m_vkLogicalDevice;
		vk::Queue m_vkGraphicsQueue;
		vk::Queue m_vkPresentQueue;

		// Internal data
		Ref<VKInstance> m_Instance;
		std::vector<PhysicalDeviceInfo> m_ListPhysicalDevices;

		std::vector<const char*> m_ListLayers;
		std::vector<const char*> m_ListExtensions;
	};


} // namespace Helios
