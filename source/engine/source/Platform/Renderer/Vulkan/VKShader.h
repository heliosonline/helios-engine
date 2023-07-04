#pragma once

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


namespace Helios {


	// Early declarations
	namespace Vulkan
	{
		class Devices;
	}


	class VKShader
	{
	public:
		VKShader(std::string name);

		void Destroy();

		vk::ShaderModule CreateModule();

	private:
		// Native vulkan objects
		vk::ShaderModule m_vkShaderModule;

		// Internal object pointers
		Ref<Vulkan::Devices> m_Devices;

		// Internal data
		std::string m_Name;
	};


} // namespace Helios
