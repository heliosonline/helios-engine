#include "pch.h"

#include "HeliosEngine/Renderer/Renderer.h"
//#include "HeliosEngine/Renderer/Renderer2D.h"


namespace Helios {


//	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();


	void Renderer::Init()
	{
		HE_PROFILER_FUNCTION();

//		RenderCommand::Init();
//		Renderer2D::Init();
	}


	void Renderer::Shutdown()
	{
		HE_PROFILER_FUNCTION();

//		Renderer2D::Shutdown();
	}


	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		HE_PROFILER_FUNCTION();

//		RenderCommand::SetViewport(0, 0, width, height);
	}


} // namespace Helios
