#include "pch.h"
#include "Model.h"

#include "HeliosEngine/Renderer/Renderer.h"

// related on build options and platform
#ifdef BUILDWITH_RENDERER_DIRECTX
#	include "Platform/Renderer/DirectX/DXModel.h"
#endif
#ifdef BUILDWITH_RENDERER_METAL
#	include "Platform/Renderer/Metal/MTModel.h"
#endif
#ifdef BUILDWITH_RENDERER_VULKAN
#	include "Platform/Renderer/Vulkan/VKModel.h"
#endif
#ifdef BUILDWITH_RENDERER_OPENGL
#	include "Platform/Renderer/OpenGL/GLModel.h"
#endif

#include "HeliosEngine/Core/Assets.h"


namespace Helios {


	Ref<Model> Model::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: LOG_CORE_EXCEPT("RendererAPI::None is not supported!"); return nullptr;

		// related on build options and platform
#		ifdef BUILDWITH_RENDERER_DIRECTX
			case RendererAPI::API::DirectX: return CreateRef<DXModel>();
#		endif
#		ifdef BUILDWITH_RENDERER_METAL
			case RendererAPI::API::Metal: return CreateRef<MTModel>();
#		endif
#		ifdef BUILDWITH_RENDERER_VULKAN
			case RendererAPI::API::Vulkan: return CreateRef<VKModel>();
#		endif
#		ifdef BUILDWITH_RENDERER_OPENGL
			case RendererAPI::API::OpenGL: return CreateRef<GLModel>();
#		endif

		default: LOG_CORE_EXCEPT("Unknown or not implemented RendererAPI!"); return nullptr;
		}
	}


	void Model::Load(const std::string& filename, const std::string& arcname)
	{
		Assets::Load(filename, arcname);
	}


} // namespace Helios
