#pragma once


class Sandbox : public Helios::Application
{
public:
	static constexpr int APP_VERSION_MAJOR = 0;
	static constexpr int APP_VERSION_MINOR = 1;
	static constexpr int APP_VERSION_PATCH = 0;
	static constexpr uint32_t APP_VERSION = HE_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

	Sandbox(const Helios::ApplicationSpecification spec);
	~Sandbox();

private:
//	Helios::Ref<Helios::Model> m_modelTest;
};
