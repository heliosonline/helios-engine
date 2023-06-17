#include "pch.h"

#include "Platform/Renderer/Metal/MTRendererAPI.h"


namespace Helios {


	void MTRendererAPI::Init()
	{
		//#ifdef BUILD_DEBUG
		//		// install a message callback for debuging
		//		glEnable(GL_DEBUG_OUTPUT);
		//		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		//
		//		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		//		LOG_CORE_DEBUG("OpenGL Renderer: DebugMessageCallback installed");
		//#endif
		//
		//		glEnable(GL_BLEND);
		//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//		glEnable(GL_DEPTH_TEST);
		//		glEnable(GL_LINE_SMOOTH);
	}


} // namespace Helios
