#include "pch.h"

#include "Platform/Renderer/Vulkan/VKRendererAPI.h"

#include "Platform/Renderer/Vulkan/Core/Instance.h"
#include "Platform/Renderer/Vulkan/Core/Devices.h"
#include "Platform/Renderer/Vulkan/Core/Swapchain.h"
#include "Platform/Renderer/Vulkan/Core/Pipeline.h"

#include "HeliosEngine/Core/Assets.h"


namespace Helios {


	void VKRendererAPI::Init()
	{
		LOG_RENDER_DEBUG("Initializing vulkan renderer...");

		Assets::Open("RendererVulkan");

		m_Instance = CreateRef<Vulkan::Instance>();
		m_Instance->Create();

		m_Devices = CreateRef<Vulkan::Devices>();
		m_Devices->Create();

		m_Swapchain = CreateRef<Vulkan::Swapchain>();
		m_Swapchain->Create();

		m_Pipeline = CreateRef<Vulkan::Pipeline>();
		m_Pipeline->Create();
	}


	void VKRendererAPI::Shutdown()
	{
		LOG_RENDER_DEBUG("Shutting down vulkan renderer...");

		m_Pipeline->Destroy();
		m_Swapchain->Destroy();
		m_Devices->Destroy();
		m_Instance->Destroy();

		Assets::Close("RendererVulkan");
	}


} // namespace Helios
