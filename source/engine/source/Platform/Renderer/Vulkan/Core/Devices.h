#pragma once

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


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


	class Instance;


	class Devices
	{
	public:
		Devices();
		~Devices() = default;

		void Create();
		void Destroy();

	public:
		vk::PhysicalDevice& GetPhysicalDevice() { return m_vkPhysicalDevice; }
		vk::Device& GetLogicalDevice() { return m_vkLogicalDevice; }
		QueueFamilyIndices GetQueueFamilies(vk::PhysicalDevice device = nullptr);

	private:
		void PickPhysicalDevice();
		std::vector<PhysicalDeviceInfo>& GetPhysicalDevices();
		int RateSuitability(const vk::PhysicalDevice& device);
		void CreateLogicalDevice();
		void GetQueues();

	private:
		// Class/Object pointers
		Ref<Instance> m_Instance;
		
		// Vulkan objects
		vk::PhysicalDevice m_vkPhysicalDevice;
		vk::Device m_vkLogicalDevice;
		vk::Queue m_vkGraphicsQueue;
		vk::Queue m_vkPresentQueue;

		// Internal data
		std::vector<PhysicalDeviceInfo> m_ListPhysicalDevices;
		std::vector<const char*> m_ListLayers;
		std::vector<const char*> m_ListExtensions;
	};


} // namespace Helios::Vulkan
