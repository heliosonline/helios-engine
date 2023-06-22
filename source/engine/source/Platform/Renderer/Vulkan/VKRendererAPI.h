#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"


namespace Helios {


	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};


	class VKRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void Shutdown() override;

	private:
		// Vulkan instance
		void CreateInstance();
		void CreateDebugMessanger();
		bool CheckSupportedInstanceLayers(const std::vector<const char*>& layers);
		bool CheckSupportedInstanceExtensions(const std::vector<const char*>& extensions);
		void CreateSurface();

		void ChoosePhysicalDevice();
		bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device);
		bool CheckSupportedDeviceExtensions(const vk::PhysicalDevice& device, const std::vector<const char*>& requested);
		QueueFamilyIndices ChooseQueueFamilies(const vk::PhysicalDevice& device);
		void CreateLogicalDevice();
		void GetQueues();


	private:
		// Instance related
		vk::Instance m_vkInstance;
		vk::DispatchLoaderDynamic m_vkLoader;
		vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
		vk::SurfaceKHR m_vkSurface;

		// Device related
		vk::PhysicalDevice m_vkPhysDevice;
		vk::Device m_vkDevice;
		vk::Queue m_vkGraphicsQueue;
		vk::Queue m_vkPresentQueue;


	};


} // namespace Helios
