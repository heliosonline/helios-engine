#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"


namespace Helios {


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

		void ChoosePhysicalDevice();
		bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device);
		bool CheckSupportedDeviceExtensions(const vk::PhysicalDevice& device, const std::vector<const char*>& requested);

	private:
		// Vulkan instance
		vk::Instance m_vkInstance;
		vk::DispatchLoaderDynamic m_vkLoader;
		vk::DebugUtilsMessengerEXT m_vkDebugMessenger;

		// Vulkan device
		vk::PhysicalDevice m_vkPhysDev;

	};


} // namespace Helios
