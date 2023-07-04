#include "pch.h"

#include "HeliosEngine/Renderer/Renderer.h"

#include "HeliosEngine/Core/Application.h"
#include "HeliosEngine/Core/Config.h"


namespace Helios {


	Ref<RendererAPI> Renderer::s_RendererAPI;


	void Renderer::Setup()
	{
		HE_PROFILER_FUNCTION();

		// Check previous used API
		std::string apistr = Config::Get("RendererAPI", "None");
		if (apistr != "None")
			LOG_RENDER_INFO("Previously used renderer API: {}.", apistr);
		if (apistr == "DirectX")
		{
#			ifdef BUILDWITH_RENDERER_DIRECTX
				RendererAPI::SetAPI(RendererAPI::API::DirectX);
#			else
				LOG_RENDER_INFO("Previously used renderer API is not supported in this build.");
#			endif
		}
		else if (apistr == "Metal")
		{
#			ifdef BUILDWITH_RENDERER_METAL
				RendererAPI::SetAPI(RendererAPI::API::Metal);
#			else
				LOG_RENDER_INFO("Previously used renderer API is not supported in this build.");
#			endif
		}
		else if (apistr == "Vulkan")
		{
#			ifdef BUILDWITH_RENDERER_VULKAN
				RendererAPI::SetAPI(RendererAPI::API::Vulkan);
#			else
				LOG_RENDER_INFO("Previously used renderer API is not supported in this build.");
#			endif
		}
		else if (apistr == "OpenGL")
		{
#			ifdef BUILDWITH_RENDERER_OPENGL
				RendererAPI::SetAPI(RendererAPI::API::OpenGL);
#			else
				LOG_RENDER_INFO("Previously used renderer API is not supported in this build.");
#			endif
		}
		else
			LOG_RENDER_INFO("Previously used renderer API \"{}\" is unknown!", apistr);

		// Check if an API is requested
		ApplicationCommandLineArgs args = Application::Get().GetSpecification().CommandLineArgs;
		if (args.Check("directx"))
		{
			LOG_RENDER_INFO("Requested renderer API from CmdArgs: DirectX.");
#			ifdef BUILDWITH_RENDERER_DIRECTX
				RendererAPI::SetAPI(RendererAPI::API::DirectX);
#			else
				LOG_RENDER_INFO("Requested renderer API is not supported in this build.");
#			endif
		}
		if (args.Check("metal"))
		{
			LOG_RENDER_INFO("Requested renderer API from CmdArgs: Metal.");
#			ifdef BUILDWITH_RENDERER_METAL
				RendererAPI::SetAPI(RendererAPI::API::Metal);
#			else
				LOG_RENDER_INFO("Requested renderer API is not supported in this build.");
#			endif
		}
		if (args.Check("vulkan"))
		{
			LOG_RENDER_INFO("Requested renderer API from CmdArgs: Vulkan.");
#			ifdef BUILDWITH_RENDERER_VULKAN
				RendererAPI::SetAPI(RendererAPI::API::Vulkan);
#			else
				LOG_RENDER_INFO("Requested renderer API is not supported in this build.");
#			endif
		}
		if (args.Check("opengl"))
		{
			LOG_RENDER_INFO("Requested renderer API from CmdArgs: OpenGL.");
#			ifdef BUILDWITH_RENDERER_OPENGL
				RendererAPI::SetAPI(RendererAPI::API::OpenGL);
#			else
				LOG_RENDER_INFO("Requested renderer API is not supported in this build.");
#			endif
		}

		// If no API is requestet select the default one
		if (RendererAPI::GetAPI() == RendererAPI::API::None)
		{
			RendererAPI::SetAPI(DEFAULT_RENDERERAPI);
			LOG_RENDER_INFO("Default renderer API: {}.", RendererAPI::GetAPIString());
		}
		LOG_RENDER_INFO("Selected renderer API: {}.", RendererAPI::GetAPIString());

		// Update config
		Config::Set("RendererAPI", RendererAPI::GetAPIString());
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
