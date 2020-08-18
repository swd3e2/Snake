#pragma once

#include "ISystem.h"
#include <entt/entt.hpp>
#include "Components.h"
#include <glm/glm.hpp>
#include <iostream>

class MoveSystem : public ISystem 
{
public:
	MoveSystem(SceneManager* sceneManager) :
		ISystem(sceneManager) 
	{}

	virtual void update(double dt) override 
	{
		entt::registry* registry = sceneManager->getCurrentScene()->getRegistry();
		auto group = registry->group<MoveComponent, TransformComponent>();

		for (auto entity : group) {
			auto& move = group.get<MoveComponent>(entity);
			auto& transform = group.get<TransformComponent>(entity);

			glm::vec3 direction = move.destination - transform.translation;
			
			if (abs(direction.x) < 0.02f || abs(direction.z) < 0.02f) {
				registry->remove<MoveComponent>(entity);
				continue;
			}

			if (transform.translation.x != move.destination.x && transform.translation.z != move.destination.z) {
				transform.translation += glm::normalize(direction) / 200.0f;
				transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
			} else {
				std::cout << "x " << move.destination.x << " y " << move.destination.y << " z " << move.destination.z << std::endl;
				std::cout << "x " << transform.translation.x << " y " << transform.translation.y << " z " << transform.translation.z << std::endl;
				registry->remove<MoveComponent>(entity);
			}
		}
	}
};