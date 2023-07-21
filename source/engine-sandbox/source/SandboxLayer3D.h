#pragma once

#include <HeliosEngine/HeliosEngine.h>


class SandboxLayer3D : public Helios::Layer
{
public:
	SandboxLayer3D();
	~SandboxLayer3D() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Helios::Timestep ts) override;
	void OnEvent(Helios::Event& e) override;
};
