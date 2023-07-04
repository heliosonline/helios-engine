#include "pch.h"

#include "HeliosEngine/Renderer/RendererAPI.h"

// related on build options and platform
#ifdef BUILDWITH_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DXRendererAPI.h"
#endif
#ifdef BUILDWITH_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTRendererAPI.h"
#endif
#ifdef BUILDWITH_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKRendererAPI.h"
#endif
#ifdef BUILDWITH_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLRendererAPI.h"
#endif


namespace Helios {


	RendererAPI::API RendererAPI::s_API = RendererAPI::API::None;


	const char* RendererAPI::GetAPIString(API api)
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    return "None";
		case RendererAPI::API::DirectX: return "DirectX";
		case RendererAPI::API::Metal:   return "Metal";
		case RendererAPI::API::Vulkan:  return "Vulkan";
		case RendererAPI::API::OpenGL:  return "OpenGL";
		default: return "Unknown";
		}
	}

	Ref<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None: LOG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;

		// related on build options and platform
#		ifdef BUILDWITH_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateRef<DXRendererAPI>();
#		endif
#		ifdef BUILDWITH_RENDERER_METAL
			case RendererAPI::API::Metal: return CreateRef<MTRendererAPI>();
#		endif
#		ifdef BUILDWITH_RENDERER_VULKAN
			case RendererAPI::API::Vulkan: return CreateRef<VKRendererAPI>();
#		endif
#		ifdef BUILDWITH_RENDERER_OPENGL
			case RendererAPI::API::OpenGL: return CreateRef<GLRendererAPI>();
#		endif

		default: LOG_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}
	}


} // namespace Helios
