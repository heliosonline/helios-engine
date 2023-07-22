#pragma once

#include "HeliosEngine/Core/UUID.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Helios::Component {


	struct ID
	{
		UUID uuid;

		ID() = default;
		ID(const ID&) = default;
		ID(const UUID uuid) : uuid(uuid) {}

		operator UUID() { return uuid; }
		operator const UUID() const { return uuid; }
	};


	struct Name
	{
		std::string name;

		Name() = default;
		Name(const Name&) = default;
		Name(const std::string& name) { name.empty() ? "Entity" : name; }

		operator std::string&() { return name; }
		operator const std::string&() const { return name; }
	};


	struct Transform
	{
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const glm::vec3& translation)
			: translation(translation) {}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), translation)
				* glm::toMat4(glm::quat(rotation))
				* glm::scale(glm::mat4(1.0f), scale);
		}

		operator glm::mat4() { return GetTransform(); }
		operator const glm::mat4() const { return GetTransform(); }
	};


} // namespace Helios::ECS::Component
