#pragma once

#ifdef BUILDWITH_RENDERER_DIRECTX
#endif
#ifdef BUILDWITH_RENDERER_METAL
#endif
#ifdef BUILDWITH_RENDERER_VULKAN
#	include <vulkan/vulkan.hpp>
#endif
#ifdef BUILDWITH_RENDERER_OPENGL
#endif


namespace Helios {


	// related on build options and platform
#	if   defined BUILDWITH_RENDERER_DIRECTX
#		define DEFAULT_RENDERERAPI RendererAPI::API::DirectX
#	elif defined BUILDWITH_RENDERER_METAL
#		define DEFAULT_RENDERERAPI RendererAPI::API::Metal
#	elif defined BUILDWITH_RENDERER_VULKAN
#		define DEFAULT_RENDERERAPI RendererAPI::API::Vulkan
#	elif defined BUILDWITH_RENDERER_OPENGL
#		define DEFAULT_RENDERERAPI RendererAPI::API::OpenGL
#	else
#		define DEFAULT_RENDERERAPI RendererAPI::API::None
#	endif


	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,

			DirectX,
			Metal,
			Vulkan,
			OpenGL
		};

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		inline static API GetAPI() { return s_API; }
		inline static void SetAPI(API api) { s_API = api; }

		static Scope<RendererAPI> Create();

	private:
		static API s_API;
	};


} // namespace Helios
