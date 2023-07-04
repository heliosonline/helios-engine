#pragma once

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


namespace Helios::Vulkan {


	class Instance
	{
	public:
		Instance() = default;
		~Instance() = default;

		void Create();
		void Destroy();

	public:
		vk::Instance& Get() { return m_vkInstance; }
		vk::SurfaceKHR& GetSurface() { return m_vkSurface; }

	private:
		bool CheckSupportedLayers();
		bool CheckSupportedExtensions();
		void CreateDebugMessanger();
		void CreateSurface();

	private:
		// Vulkan objects
		vk::Instance m_vkInstance;
		vk::DispatchLoaderDynamic m_vkLoader;
		vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
		vk::SurfaceKHR m_vkSurface;

		// Internal data
		std::vector<const char*> m_ListLayers;
		std::vector<const char*> m_ListExtensions;
	};


} // namespace Helios::Vulkan
