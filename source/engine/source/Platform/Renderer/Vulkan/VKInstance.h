#pragma once

#include <vulkan/vulkan.hpp>


namespace Helios {


	class VKInstance
	{
	public:

		void Create();
		void Destroy();

		vk::Instance& GetInstance() { return m_vkInstance; }
		vk::SurfaceKHR& GetSurface() { return m_vkSurface; }

	private:
		bool CheckSupportedLayers();
		bool CheckSupportedExtensions();
		void CreateDebugMessanger();
		void CreateSurface();

	private:
		// Native vulkan objects
		vk::Instance m_vkInstance;
		vk::DispatchLoaderDynamic m_vkLoader;
		vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
		vk::SurfaceKHR m_vkSurface;

		// Internal data
		std::vector<const char*> m_ListLayers;
		std::vector<const char*> m_ListExtensions;
	};


} // namespace Helios
