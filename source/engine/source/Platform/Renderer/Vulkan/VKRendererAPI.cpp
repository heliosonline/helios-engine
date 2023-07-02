#include "pch.h"

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"


namespace Helios {


	void VKRendererAPI::Init()
	{
		LOG_RENDER_DEBUG("Initializing vulkan renderer...");

		m_Instance = CreateRef<VKInstance>();
		m_Instance->Create();

		m_Device = CreateRef<VKDevice>(m_Instance);
		m_Device->Create();

		m_SwapChain = CreateRef<VKSwapChain>(m_Instance, m_Device);
		m_SwapChain->Create();

		m_Pipeline = CreateRef<VKPipeline>();
		m_Pipeline->Create();
	}


	void VKRendererAPI::Shutdown()
	{
		LOG_RENDER_DEBUG("Shutting down vulkan renderer...");

		if (m_Pipeline)
			m_Pipeline->Destroy();
		if (m_SwapChain)
			m_SwapChain->Destroy();
		if (m_Device)
			m_Device->Destroy();
		if (m_Instance)
			m_Instance->Destroy();
	}


} // namespace Helios
