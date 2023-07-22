#include "pch.h"
#include "Sandbox.h"

#include "SandboxLayer3D.h"


Helios::Application* Helios::CreateApplication(Helios::ApplicationCommandLineArgs args)
{
	Helios::ApplicationSpecification spec{};
	{
		spec.Name = "Helios-Sandbox";
		spec.Version = Sandbox::APP_VERSION;
		spec.CommandLineArgs = args;
		spec.hints |= Helios::Hints::HINT_USE_EXEPATH_AS_CWD;
		spec.logfile = "Helios-Sandbox.log";
		spec.configfile = "Helios-Sandbox.cfg";
	};

	return new Sandbox(spec);
}


Sandbox::Sandbox(const Helios::ApplicationSpecification spec)
	: Helios::Application(spec)
{
	PushLayer(new SandboxLayer3D());
//	PushOverlay(new ClientUILayer());

//	m_modelTest = Helios::Model::Create();
}


Sandbox::~Sandbox()
{
}
