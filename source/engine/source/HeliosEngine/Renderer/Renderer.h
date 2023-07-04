#pragma once

#include "HeliosEngine/Renderer/RendererAPI.h"


namespace Helios {


	class Renderer
	{
	public:
		static void Setup();
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		static RendererAPI* Get() { return s_RendererAPI.get(); }

	private:
		static Ref<RendererAPI> s_RendererAPI;
	};


} // namespace Helios
