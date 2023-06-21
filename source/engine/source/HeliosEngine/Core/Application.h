#pragma once

#include "HeliosEngine/Core/Layer.h"
#include "HeliosEngine/Core/Window.h"

#include "HeliosEngine/Events/Event.h"
#include "HeliosEngine/Events/ApplicationEvent.h"

//#include "HeliosEngine/ImGui/ImGuiLayer.h"


namespace Helios {


	int AppMain(int argc, char** argv);


	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			LOG_CORE_ASSERT(index < Count, "");
			return Args[index];
		}

		bool Check(std::string arg);
		std::string Get(std::string arg);
	};

	enum Hints
	{
		HINT_USE_CWD            = (1 << 0), // use the current work dir as base path
		HINT_USE_EXEPATH_AS_CWD = (1 << 1), // use path of executeable as base path
	};

	struct ApplicationSpecification
	{
		// Name/Title of Application
		std::string Name;
		// Version of Application
		uint32_t Version;
		// Base path for FileIO
		std::string WorkingDirectory;
		// Command Line Arguments
		ApplicationCommandLineArgs CommandLineArgs;
		// Basic configuration hints
		unsigned int hints = 0;
		// Name of the logfile
		std::string logfile = "HeliosEngine.log";
		// Name of the configfile
		std::string configfile = "HeliosEngine.cfg";
	};


	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		static Application& Get() { return *s_Instance; }
		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_Window; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
//		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
		friend int AppMain(int argc, char** argv);
	};


	// Needs to be defined/implemented in Client/Application
	Application* CreateApplication(ApplicationCommandLineArgs args);


} // namespace Helios
