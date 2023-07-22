#include "pch.h"
#include "ECS.h"

#include "HeliosEngine/ECS/Entity.h"


// TODO: note for myself...
// ref -> https://www.youtube.com/watch?v=D4hz0wEB978
// ref -> https://www.youtube.com/watch?v=-B1iu4QJTUc&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=78


namespace Helios::ECS {


	entt::registry s_Registry{};


	Entity Create(const UUID uuid, const std::string& name)
	{
		Entity entity = { s_Registry.create()};

		entity.AddComponent<Component::ID>(uuid);
		entity.AddComponent<Component::Transform>();
		auto& cname = entity.AddComponent<Component::Name>(name);

//		m_EntityMap[uuid] = entity;

		return entity;
	}


	entt::registry& Registry()
	{
		return s_Registry;
	}


} // namespace Helios::ECS
