#pragma once



namespace Helios {

	class Entity;
	class UUID;

	class ECS
	{
	public:
		static entt::registry& Registry();

		static Entity Create(const std::string& name = "");
		static Entity Create(UUID uuid, const std::string& name = "");
		static Entity Duplicate(Entity& entity);
		static void Destroy(Entity& entity);

		static Entity& FindByName(const std::string& name);
		static Entity& FindByUUID(UUID uuid);

		template<typename... Components>
		static auto GetAllWith()
		{
			return s_Registry.view<Components...>();
		}

	private:
		static entt::registry s_Registry;
		static std::unordered_map<UUID, entt::entity> s_EntityMap;
	};


} // namespace Helios
