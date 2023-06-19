#include "pch.h"

#include "HeliosEngine/Renderer/Renderer.h"
//#include "HeliosEngine/Renderer/Renderer2D.h"


namespace Helios {


//	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();


	void Renderer::Init()
	{
		HE_PROFILER_FUNCTION();

		s_RendererAPI->Init();
	}


	void Renderer::Shutdown()
	{
		HE_PROFILER_FUNCTION();

		s_RendererAPI->Shutdown();
	}


	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		HE_PROFILER_FUNCTION();

//		RenderCommand::SetViewport(0, 0, width, height);
	}


} // namespace Helios
