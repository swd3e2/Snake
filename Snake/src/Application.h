#pragma once

#include <chrono>

#include "Components.h"
#include "Interface/ImGuiInterface.h"
#include "Import/TextureLoader.h"
#include "Import/ModelLoader.h"
#include "Configuration.h"
#include "Common/Helper.h"
#include "EventSystem/EventSystem.h"
#include "JobSystem.h"
#include "ApplicationSettings.h"
#include "Systems/SystemManager.h"
#include "Scene/SceneManager.h"

class Application {
private:
	ApplicationSettings settings;
	SystemManager systemManager;
	SceneManager sceneManager;

	std::unique_ptr<EventSystem> eventSystem;

	Renderer* renderer;
	std::shared_ptr<ImGuiInterface> minterface;

	std::chrono::time_point<std::chrono::steady_clock> m_Start;

	TextureLoader* textureLoader;
    ModelLoader* modelLoader;
	Configuration* applicationRegistry;

	Window* window;
	JobSystem jobSystem;
	std::vector<WorkerThread*> threads;
public:
	Application() 
	{
		applicationRegistry = Singleton<Configuration>::startUp();
		textureLoader = Singleton<TextureLoader>::startUp();
		modelLoader = Singleton<ModelLoader>::startUp();

		eventSystem = std::make_unique<EventSystem>();
		renderer = Renderer::create(RendererType::DirectX);
		window = renderer->createWindow(settings.getWindowWidth(), settings.getWindowHeight());

		systemManager.initialize(&settings, &sceneManager, renderer);
		sceneManager.loadSceneFromFile("test.scene");
		initSystems();
	}

	void run() {
        double dt = 0.0;
        
        while (window->isOpen()) {
            m_Start = std::chrono::high_resolution_clock::now();

			eventSystem->flush();
			systemManager.update(dt);
			
			//minterface->update(dt);

			InputManager::instance()->mouseMoveX = 0.0;
			InputManager::instance()->mouseMoveY = 0.0;
			InputManager::instance()->mouseMoveCnt = 0;
			window->pollEvents();

            dt = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_Start).count();
        }
	}

	void initSystems()
	{
		entt::registry* registry = sceneManager.getCurrentScene()->getRegistry();
		{
			entt::entity entity = registry->create();
			TransformComponent& transform = registry->emplace<TransformComponent>(entity);
			transform.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			registry->emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
		}
		{
			entt::entity entity = registry->create();
			TransformComponent& transform = registry->emplace<TransformComponent>(entity);
			transform.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
			registry->emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
		}
		{
			entt::entity entity = registry->create();
			TransformComponent& transform = registry->emplace<TransformComponent>(entity);
			transform.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
			registry->emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
		}
	}

	void createBoxes() 
	{
		/*entt::entity entity;
		for (int i = 0; i < 10; i++) {
			entity = registry.create();	
			btCollisionShape* shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
			PhysicsComponent& physics = physicsSystem->createPhysicsBody(entity, shape, 1.0f, glm::vec3(0.0, 20.0f + i, 0.0f), true);
			TransformComponent& transform = registry.emplace<TransformComponent>(entity);
			transform.translation = glm::vec3(0.0, 20.0f + i, 0.0f);
			transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
			registry.emplace<RenderComponent>(entity, ModelLoader::instance()->loadFromFile("BoxTextured.gltf"));
		}*/
	}
};