#include "pch.h"

#include "Platform/Renderer/OpenGL/GLRendererAPI.h"

//#include "glad/gl.h"


namespace Helios {


//	void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message, const void* userParam)
//	{
//		switch (severity)
//		{
//		case GL_DEBUG_SEVERITY_HIGH:         LOG_CORE_FATAL(message); return;
//		case GL_DEBUG_SEVERITY_MEDIUM:       LOG_CORE_ERROR(message); return;
//		case GL_DEBUG_SEVERITY_LOW:          LOG_CORE_WARN(message); return;
//		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_CORE_TRACE(message); return;
//		default: LOG_CORE_ASSERT(false, "Unknown severity level!");
//		}
//	}


	void GLRendererAPI::Init()
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
