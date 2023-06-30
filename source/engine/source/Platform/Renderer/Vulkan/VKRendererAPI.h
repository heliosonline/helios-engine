#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"

#include "Platform/Renderer/Vulkan/VKInstance.h"
#include "Platform/Renderer/Vulkan/VKDevice.h"
#include "Platform/Renderer/Vulkan/VKSwapChain.h"


namespace Helios {


	class VKRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void Shutdown() override;


	private:
		Ref<VKInstance> m_Instance;
		Ref<VKDevice> m_Device;
		Ref<VKSwapChain> m_SwapChain;
	};


} // namespace Helios
