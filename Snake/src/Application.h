#pragma once

#include "RenderSystem.h"
#include "Components.h"
#include <chrono>
//#include "Interface/MainInterface.h"
#include "Model/Import/GltfImporter.h"
#include "Model/Model.h"
#include "ScriptSystem.h"
#include "Saver.h"
#include "Loader.h"
#include "Physics/PhysicsSystem.h"
#include "CameraSystem.h"
#include "PlayerSystem.h"
#include "Storage.h"
#include "TextureLoader.h"
#include "ModelLoader.h"

class Application {
private:
    Window* window;
    entt::registry registry;

    //std::shared_ptr<MainInterface> interface;
	std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<ScriptSystem> scriptSystem;
    std::unique_ptr<PhysicsSystem> physicsSystem;
	std::unique_ptr<CameraSystem> cameraSystem;
	std::unique_ptr<PlayerSystem> playerSystem;
	std::shared_ptr<Renderer> renderer;

	std::chrono::time_point<std::chrono::steady_clock> m_Start;

	TextureLoader textureLoader;
    ModelLoader modelLoader;
public:
    ~Application() {

    }

	void init() {
		textureLoader.startUp();
		modelLoader.startUp();

        renderer.reset(Renderer::create(RendererType::DirectX));
        window = renderer->createWindow(1920, 1080);

		physicsSystem = std::make_unique<PhysicsSystem>(&registry);
        renderSystem = std::make_unique<RenderSystem>(&registry, physicsSystem.get());
        scriptSystem = std::make_unique<ScriptSystem>(&registry);
		cameraSystem = std::make_unique<CameraSystem>(&registry);
		playerSystem = std::make_unique<PlayerSystem>(&registry);

        //interface = std::make_shared<MainInterface>(window, &registry, &renderSystem->camera, renderSystem.get());

        Loader loader;
        loader.loadFromFile("test.json", &registry);

        entt::entity player = registry.create();
        CameraComponent& component = registry.emplace<CameraComponent>(player);
        component.projectionMatrix = glm::perspective(glm::radians(90.0f), 1920.0f / 1080.0f, 0.01f, 200.0f);
		registry.emplace<PlayerComponent>(player);
		registry.emplace<Transform>(player);

		std::shared_ptr<btCollisionShape> shape = std::make_shared<btCapsuleShape>(0.3f, 2.6f);
		physicsSystem->createPhysicsBody(player, shape, 3.0f, glm::vec3(0.0, 20.0f, 0.0f), true);

		/*for (int i = 0; i < 100; i++) {
			entt::entity entity = registry.create();
			std::shared_ptr<btCollisionShape> shape = std::make_shared<btBoxShape>(btVector3(0.5, 0.5, 0.5));
			physicsSystem->createPhysicsBody(entity, shape, 1.0f, glm::vec3(0.0, 20.0f + i, 0.0f), true);

			registry.emplace<Transform>(entity);
			GltfImporter importer;
			std::shared_ptr<Import::Model> model = importer.import("BoxTextured.gltf");
			registry.emplace<Render>(entity, std::make_shared<Model>(model));
		}*/
		{
			entt::entity entity = registry.create();
			std::shared_ptr<btCollisionShape> shape = std::make_shared<btStaticPlaneShape>(btVector3(0, 1, 0), 0);
			physicsSystem->createPhysicsBody(entity, shape, 0.0f, glm::vec3(0.0, -0.63f, 0.0f), false);
		}
	}

	void run() {
        double dt = 0.0;
        
        while (window->isOpen()) {
            m_Start = std::chrono::high_resolution_clock::now();

            renderSystem->update(dt);
            //interface->update(dt);
            scriptSystem->update();
            physicsSystem->update(dt);
            cameraSystem->update(dt);
			playerSystem->update(dt);

			

            InputManager::instance()->mouseMoveX = 0.0;
            InputManager::instance()->mouseMoveY = 0.0;

			window->pollEvents();

            dt = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_Start).count();
        }
	}
};