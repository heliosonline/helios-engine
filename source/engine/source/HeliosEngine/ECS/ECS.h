#pragma once



namespace Helios {

	class Entity;

	namespace ECS {


		extern Entity Create(const UUID uuid, const std::string& name = "");
		extern entt::registry& Registry();


	} // namespace ECS
} // namespace Helios
