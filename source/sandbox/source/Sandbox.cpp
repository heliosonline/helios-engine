#include "pch.h"

//#include "Config/Version.h"

//#include "App/ClientLayer.h"
//#include "App/ClientUILayer.h"


class HeliosSandbox : public Helios::Application
{
public:
	HeliosSandbox(const Helios::ApplicationSpecification& specification)
		: Helios::Application(specification)
	{
//		LOG_INFO("Helios-GameClient v{0}.{1}.{2}.{3}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD);

//		PushLayer(new ClientLayer());
//		PushOverlay(new ClientUILayer());
	}

	~HeliosSandbox()
	{
	}
};


Helios::Application* Helios::CreateApplication(Helios::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Helios-Sandbox";
	spec.Version = HE_MAKE_VERSION(0, 0, 1, 0);
	spec.CommandLineArgs = args;
	spec.hints |= Helios::Hints::HINT_USE_EXEPATH_AS_CWD;
	spec.logfile = "Helios-Sandbox.log";
	spec.configfile = "Helios-Sandbox.cfg";

	return new HeliosSandbox(spec);
}
