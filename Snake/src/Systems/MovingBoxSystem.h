#pragma once

#include "ISystem.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "Components.h"

class MovingBoxSystem : public ISystem
{
public:
	MovingBoxSystem(SceneManager* sceneManager) : 
		ISystem(sceneManager) 
	{}

	virtual void update(double dt) override
	{
		entt::registry* registry = sceneManager->getCurrentScene()->getRegistry();

		registry->view<MovingBoxComponent, PhysicsComponent>().each([dt](MovingBoxComponent& movingBox, PhysicsComponent& physics) {
			btTransform transform = physics.body->getWorldTransform();
			btVector3 position = transform.getOrigin();

			float curY = position.y();
			if (curY > 20.0f) {
				movingBox.moveUp = false;
			} else if (curY < -20.0f) {
				movingBox.moveUp = true;
			}

			if (movingBox.moveUp) {
				curY += 0.01f * dt;
			} else {
				curY -= 0.01f * dt;
			}

			position.setY(curY);
			transform.setOrigin(position);
			physics.body->setWorldTransform(transform);
		});
	}
};