#include "pch.h"
#include "ECS.h"

#include "HeliosEngine/Core/UUID.h"
#include "HeliosEngine/ECS/Entity.h"


// TODO: note for myself...
// ref -> https://www.youtube.com/watch?v=D4hz0wEB978
// ref -> https://www.youtube.com/watch?v=-B1iu4QJTUc&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=78


namespace Helios {


	entt::registry ECS::s_Registry;
	std::unordered_map<UUID, entt::entity> ECS::s_EntityMap;


	entt::registry& ECS::Registry()
	{
		return s_Registry;
	}


	Entity ECS::Create(const std::string& name)
	{
		return Create(UUID(), name);
	}


	Entity ECS::Create(UUID uuid, const std::string& name)
	{
		Entity entity = { s_Registry.create()};

		entity.AddComponent<Component::ID>(uuid);
		entity.AddComponent<Component::Transform>();
		auto& cname = entity.AddComponent<Component::Name>(name);

		s_EntityMap[uuid] = entity;

		return entity;
	}


	void ECS::Destroy(Entity& entity)
	{
		s_EntityMap.erase(entity.GetUUID());
		s_Registry.destroy(entity);
	}


} // namespace Helios
