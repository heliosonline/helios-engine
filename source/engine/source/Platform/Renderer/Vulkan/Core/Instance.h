#pragma once

#include <vulkan/vulkan.hpp>


namespace Helios::Vulkan {


	class Instance
	{
	public:
		Instance();
		~Instance();

		void Create();
		void Destroy();

	// Getter for vulkan objects
	public:
		vk::Instance& GetInstance() { return m_vkInstance; }
		vk::SurfaceKHR& GetSurface() { return m_vkSurface; }

	// Vulkan objects
	private:
		vk::Instance m_vkInstance;
		vk::SurfaceKHR m_vkSurface;
		vk::DispatchLoaderDynamic m_vkLoader;
		vk::DebugUtilsMessengerEXT m_vkDebugMessenger;

	// Internal helper
	private:
		std::vector<const char*> GetRequiredLayers();
		std::vector<const char*> GetRequiredExtensions();
		bool CheckSupportedLayers();
		bool CheckSupportedExtensions();
		void CreateDebugMessenger();
		void CreateSurface();
	};


} // namespace Helios::Vulkan
