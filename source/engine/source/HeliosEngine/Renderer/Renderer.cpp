#include "pch.h"

#include "HeliosEngine/Renderer/Renderer.h"

#include "HeliosEngine/Core/Application.h"
#include "HeliosEngine/Core/Config.h"


namespace Helios {


	Scope<RendererAPI> Renderer::s_RendererAPI;


	void Renderer::Setup()
	{
		HE_PROFILER_FUNCTION();

		// Check previous used API
		std::string apistr = Config::Get("RendererAPI", "None");
#		ifdef BUILDWITH_RENDERER_DIRECTX
		if (apistr == "DirectX")
			RendererAPI::SetAPI(RendererAPI::API::DirectX);
#		endif
#		ifdef BUILDWITH_RENDERER_METAL
		if (apistr == "Metal")
			RendererAPI::SetAPI(RendererAPI::API::Metal);
#		endif
#		ifdef BUILDWITH_RENDERER_VULKAN
		if (apistr == "Vulkan")
			RendererAPI::SetAPI(RendererAPI::API::Vulkan);
#		endif
#		ifdef BUILDWITH_RENDERER_OPENGL
		if (apistr == "OpenGL")
			RendererAPI::SetAPI(RendererAPI::API::OpenGL);
#		endif

		// Check if an API is requested
		ApplicationCommandLineArgs args = Application::Get().GetSpecification().CommandLineArgs;
#		ifdef BUILDWITH_RENDERER_DIRECTX
			if (args.Check("directx"))
			{
				RendererAPI::SetAPI(RendererAPI::API::DirectX);
				LOG_RENDER_INFO("Requested renderer API from CmdArgs: DirectX.");
			}
#		endif
#		ifdef BUILDWITH_RENDERER_METAL
			if (args.Check("metal"))
			{
				RendererAPI::SetAPI(RendererAPI::API::Metal);
				LOG_RENDER_INFO("Requested renderer API from CmdArgs: Metal.");
	}
#		endif
#		ifdef BUILDWITH_RENDERER_VULKAN
			if (args.Check("vulkan"))
			{
				RendererAPI::SetAPI(RendererAPI::API::Vulkan);
				LOG_RENDER_INFO("Requested renderer API from CmdArgs: Vulkan.");
			}
#		endif
#		ifdef BUILDWITH_RENDERER_OPENGL
			if (args.Check("opengl"))
			{
				RendererAPI::SetAPI(RendererAPI::API::OpenGL);
				LOG_RENDER_INFO("Requested renderer API from CmdArgs: OpenGl.");
			}
#		endif

		// If no API is requestet select the default one
		if (RendererAPI::GetAPI() == RendererAPI::API::None)
			RendererAPI::SetAPI(DEFAULT_RENDERERAPI);

		// Update config
		if (RendererAPI::GetAPI() == RendererAPI::API::DirectX)
			Config::Set("RendererAPI", "DirectX");
		if (RendererAPI::GetAPI() == RendererAPI::API::Metal)
			Config::Set("RendererAPI", "Metal");
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			Config::Set("RendererAPI", "Vulkan");
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			Config::Set("RendererAPI", "OpenGl");
	}


	void Renderer::Init()
	{
		HE_PROFILER_FUNCTION();

		s_RendererAPI = RendererAPI::Create();
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
