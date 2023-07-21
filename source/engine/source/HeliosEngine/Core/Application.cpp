#include "pch.h"

#include "HeliosEngine/Core/Application.h"
#include "HeliosEngine/Core/Config.h"
#include "HeliosEngine/Core/EntryPoint.h"
#include "HeliosEngine/Core/Timer.h"
#include "HeliosEngine/Core/Timestep.h"
#include "HeliosEngine/Core/Util.h"
#include "HeliosEngine/Core/Assets.h"

#include "HeliosEngine/Renderer/Renderer.h"

//#include <GLFW/glfw3.h>
//#include <imgui.h>


namespace Helios {


	int AppMain(int argc, char** argv)
	{
		auto app = CreateApplication({ argc, argv });
		app->Run();
		delete app;

		return 0;
	}


	bool ApplicationCommandLineArgs::Check(std::string arg)
	{
		for (auto x = 1; x < Count; x++)
		{
			size_t start = std::string(Args[x]).find_first_not_of("/-");
			std::string trimmed = std::string(Args[x]).substr(start);
			trimmed = trimmed.substr(0, trimmed.find_first_of('='));

			std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(),
				[](unsigned char c) { return std::tolower(c); } );

			if (trimmed == arg)
				return true;
		}
		return false;
	}

	std::string ApplicationCommandLineArgs::Get(std::string arg)
	{
		for (auto x = 1; x < Count; x++)
		{
			size_t start = std::string(Args[x]).find_first_not_of("/-");
			std::string trimmed = std::string(Args[x]).substr(start);

			std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(),
				[](unsigned char c) { return std::tolower(c); });

			std::string key = trimmed.substr(0, trimmed.find_first_of('='));
			if (key.compare(arg) == 0)
				return trimmed.substr(trimmed.find_first_of('=') + 1);
		}
		return "";
	}


	Application* Application::s_Instance = nullptr;


	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		// Init working directory
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);
		if (m_Specification.hints & Hints::HINT_USE_CWD)
			m_Specification.WorkingDirectory = std::filesystem::current_path().string();
		if (m_Specification.hints & Hints::HINT_USE_EXEPATH_AS_CWD)
		{
			m_Specification.WorkingDirectory = GetExePath();
			std::filesystem::current_path(m_Specification.WorkingDirectory);
		}

		// Init logging
		Log::Init(m_Specification.logfile, m_Specification.WorkingDirectory);
		LOG_CORE_INFO("Logging started.");

		// Log versions
		LOG_CORE_DEBUG("Working path: {0}", m_Specification.WorkingDirectory);
		LOG_CORE_INFO("Engine-Version: {}.{}.{} ({})",
			HE_VERSION_MAJOR(HE_VERSION),
			HE_VERSION_MINOR(HE_VERSION),
			HE_VERSION_PATCH(HE_VERSION),
			HE_VERSION_TYPE_STRING(HE_VERSION));
		LOG_CORE_INFO("Application-Version: {}.{}.{} ({})",
			HE_VERSION_MAJOR(m_Specification.Version),
			HE_VERSION_MINOR(m_Specification.Version),
			HE_VERSION_PATCH(m_Specification.Version),
			HE_VERSION_TYPE_STRING(m_Specification.Version));

		// Check singleton
		LOG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Log CmdArgs
		if (m_Specification.CommandLineArgs.Count > 1)
		{
			for (auto x = 1; x < m_Specification.CommandLineArgs.Count; x++)
				LOG_CORE_INFO("CmdArg[{}] = \"{}\"", x, m_Specification.CommandLineArgs[x]);;
		}

		// Load config
		Config::Init(m_Specification.configfile, m_Specification.WorkingDirectory);

		Assets::Init(m_Specification.WorkingDirectory);

		// Init Window/renderer
		Renderer::Setup();
		m_Window = Window::Create(WindowSpecification(m_Specification.Name));
		m_Window->SetEventCallback(HE_BIND_EVENT_FN(Application::OnEvent));
		Renderer::Init();

//		static std::string inipath = m_Specification.WorkingDirectory;
//		inipath += "/imgui.ini";
//		m_ImGuiLayer = new ImGuiLayer(inipath);
//		PushOverlay(m_ImGuiLayer);
	}


	Application::~Application()
	{
		LOG_CORE_INFO("App Shutdown.");

		Config::Save();
		Renderer::Shutdown();
	}


	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();

		int size_x, size_y;
		glfwGetWindowSize((GLFWwindow*)m_Window->GetNativeWindow(), &size_x, &size_y);
		WindowResizeEvent event(size_x, size_y);
		OnEvent(event);
	}


	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();

		int size_x, size_y;
		glfwGetWindowSize((GLFWwindow*)m_Window->GetNativeWindow(), &size_x, &size_y);
		WindowResizeEvent event(size_x, size_y);
		OnEvent(event);
	}


	void Application::Close()
	{
		m_Running = false;
	}


	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HE_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<FramebufferResizeEvent>(HE_BIND_EVENT_FN(Application::OnFramebufferResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		Timer RunLoopTimer;
		while (m_Running)
		{
			Timestep timestep = RunLoopTimer.Elapsed();
			RunLoopTimer.Reset();

			{ // tempoary for debuging
				static int fps = 0;
				fps += (int)(1.0f / timestep);
				static int fps_cnt = 0;
				fps_cnt++;
				static float fps_ts = 0;
				fps_ts += timestep;
				if (fps_ts >= 1.0f)
				{
					std::ostringstream title;
					title << "FPS: " << fps / fps_cnt << " (" << 1000 * fps_ts / fps_cnt << " ms)";
					glfwSetWindowTitle((GLFWwindow*)m_Window->GetNativeWindow(), title.str().c_str());
					fps = 0;
					fps_cnt = 0;
					fps_ts = 0;
				}
			} // tempoary for debuging

			Renderer::Get()->Render();


//			{
//				static double lastTime, currentTime;
//				static int numFrames;
//				static float frameTime;
//
//				currentTime = glfwGetTime();
//				double delta = currentTime - lastTime;
//
//				if (delta >= 1)
//				{
//					int framerate{ std::max(1, int(numFrames / delta)) };
//					std::stringstream title;
//					title << "Running at " << framerate << " fps.";
//					glfwSetWindowTitle((GLFWwindow*)m_Window->GetNativeWindow(), title.str().c_str());
//					lastTime = currentTime;
//					numFrames = -1;
//					frameTime = float(1000.0 / framerate);
//				}
//				++numFrames;
//			}


			if (!m_Minimized)
			{
				{
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

//				{
//					m_ImGuiLayer->Begin();
//					//static bool show = true;
//					//ImGui::ShowDemoWindow(&show);
//					for (Layer* layer : m_LayerStack)
//						layer->OnImGuiRender();
//					m_ImGuiLayer->End();
//				}
			}

			{
				m_Window->OnUpdate();
			}
		}
	}


	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}


	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}


	bool Application::OnFramebufferResize(FramebufferResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnFramebufferResize(e.GetWidth(), e.GetHeight());

		return false;
	}

} // namespace Helios
