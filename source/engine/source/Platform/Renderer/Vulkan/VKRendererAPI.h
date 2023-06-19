#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"


namespace Helios {


	class VKRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void Shutdown() override;

	private:
		void CreateInstance();
		void CreateDebugMessanger();

		bool CheckSupportedLayers(std::vector<const char*>& layers);
		bool CheckSupportedExtensions(std::vector<const char*>& extensions);

		vk::Instance m_vkInstance;
		vk::DispatchLoaderDynamic m_vkLoader;
		vk::DebugUtilsMessengerEXT m_vkDebugMessenger;

	};


} // namespace Helios
