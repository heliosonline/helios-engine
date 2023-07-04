#include "pch.h"

#include "Platform/Renderer/Vulkan/VKShader.h"
#include "HeliosEngine/Renderer/Renderer.h"

#include "Platform/Renderer/Vulkan/Core/Devices.h"

#include "HeliosEngine/Core/Assets.h"


namespace Helios {


	VKShader::VKShader(std::string name)
		: m_Name(name)
	{
		m_Devices = ((VKRendererAPI*)Renderer::Get())->GetDevices();
	}


	void VKShader::Destroy()
	{
		if (m_vkShaderModule)
			m_Devices->GetLogicalDevice().destroyShaderModule(m_vkShaderModule);
	}


	vk::ShaderModule VKShader::CreateModule()
	{
		// Load the source code
		std::vector<char> source = Assets::Load("Shader/" + m_Name, "RendererVulkan");

		// Setup ModuleInfo
		vk::ShaderModuleCreateInfo ModuleInfo = vk::ShaderModuleCreateInfo();
		{
			ModuleInfo.codeSize = source.size();
			ModuleInfo.pCode = reinterpret_cast<const uint32_t*>(source.data());
		}

		// Create module
		try {
			m_vkShaderModule = m_Devices->GetLogicalDevice().createShaderModule(ModuleInfo);
		}
		catch (vk::SystemError err) {
			LOG_RENDER_ASSERT(0, "Failed to create shader module!");
		}
		return m_vkShaderModule;
	}


} // namespace Helios
