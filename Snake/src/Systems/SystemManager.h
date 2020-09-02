#pragma once

#include <memory>
#include "ApplicationSettings.h"
#include "ScriptSystem.h"
#include "RenderSystem.h"
#include "CameraSystem.h"
#include "PlayerSystem.h"
#include "MovingBoxSystem.h"
#include "MoveSystem.h"
#include "GltfTransformSystem.h"
#include "Scene/SceneManager.h"
#include "Graphics/Renderer.h"
#include "EventSystem/EventSystem.h"
#include "GltfTransformSystem.h"

class SystemManager 
{
private:
	std::unique_ptr<RenderSystem> renderSystem;
	std::unique_ptr<ScriptSystem> scriptSystem;
	std::unique_ptr<PhysicsSystem> physicsSystem;
	std::unique_ptr<CameraSystem> cameraSystem;
	std::unique_ptr<MovingBoxSystem> movingBoxSystem;
	std::unique_ptr<PlayerSystem> playerSystem;
	std::unique_ptr<MoveSystem> moveSystem;
	std::unique_ptr<GltfTransformSystem> gltfTransformSystem;
public:
	void initialize(ApplicationSettings* settings, SceneManager* sceneManager, Renderer* renderer)
	{
		physicsSystem	= std::make_unique<PhysicsSystem>(sceneManager);
		renderSystem	= std::make_unique<RenderSystem>(sceneManager, settings, physicsSystem.get(), renderer);
		scriptSystem	= std::make_unique<ScriptSystem>(sceneManager);
		cameraSystem	= std::make_unique<CameraSystem>(sceneManager);
		playerSystem	= std::make_unique<PlayerSystem>(sceneManager, physicsSystem.get());
		//moveSystem		= std::make_unique<MoveSystem>(sceneManager);
		//movingBoxSystem = std::make_unique<MovingBoxSystem>(sceneManager);
		gltfTransformSystem = std::make_unique<GltfTransformSystem>(sceneManager);
		//playerSystem->mainCamera = &renderSystem->camera;
	}

	void update(double dt)
	{
		//jobSystem.addJob({ [physicsSystem = physicsSystem.get(), dt = dt] () { physicsSystem->update(dt); } });;
		physicsSystem->update(dt);

		gltfTransformSystem->update(dt);
		scriptSystem->update(dt);
		cameraSystem->update(dt);
		playerSystem->update(dt);

		/*moveSystem->update(dt);
		movingBoxSystem->update(dt);
		while (!jobSystem.jobsCompleted());*/
		renderSystem->update(dt);
	}
};