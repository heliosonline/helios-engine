#pragma once

#include "HeliosEngine/ECS/ECS.h"
#include "HeliosEngine/ECS/Components.h"


namespace Helios {


	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle) : m_handle(handle) {}
		Entity(const Entity& other) = default;
		~Entity() { ECS::Registry().destroy(m_handle); }


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			LOG_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			entt::registry& reg = ECS::Registry();
			T& component = reg.emplace<T>(m_handle, std::forward<Args>(args)...);
//			ECS::OnComponentAdded<T>(*this, component);
			return component;
		}


		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = ECS::Registry().emplace_or_replace<T>(m_handle, std::forward<Args>(args)...);
//			ECS::OnComponentAdded<T>(*this, component);
			return component;
		}


		template<typename T>
		T& GetComponent()
		{
			LOG_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return ECS::Registry().get<T>(m_handle);
		}


		template<typename T>
		bool HasComponent()
		{
			return ECS::Registry().all_of<T>(m_handle);
		}


		template<typename T>
		void RemoveComponent()
		{
			LOG_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			ECS::Registry().remove<T>(m_handle);
		}

	// Operators
	public:
		operator bool() const { return m_handle != entt::null; }
		operator entt::entity() const { return m_handle; }
		operator uint32_t() const { return (uint32_t)m_handle; }

		bool operator==(const Entity& other) const
		{
			return m_handle == other.m_handle;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	// Helper
	public:
		UUID GetUUID() { return GetComponent<Component::ID>().uuid; }
		const std::string& GetName() { return GetComponent<Component::Name>().name; }

	private:
		entt::entity m_handle{ entt::null };
	};


} // namespace Helios
