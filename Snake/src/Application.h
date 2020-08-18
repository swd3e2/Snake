#pragma once

#include "Components.h"
#include <chrono>
#include "Interface/ImGuiInterface.h"
#include "Model/Model.h"

#include "Systems/PhysicsSystem.h"
#include "Import/TextureLoader.h"
#include "Import/ModelLoader.h"
#include "Configuration.h"
#include <Windows.h>
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
		applicationRegistry		= Singleton<Configuration>::startUp();
		textureLoader			= Singleton<TextureLoader>::startUp();
		modelLoader				= Singleton<ModelLoader>::startUp();

		saveRuntimeDir();

		eventSystem = std::make_unique<EventSystem>();
		renderer = Renderer::create(RendererType::DirectX);
		window = renderer->createWindow(
			settings.getWindowWidth(), 
			settings.getWindowHeight(),
			eventSystem.get()
		);

		systemManager.initialize(&settings, &sceneManager, renderer, eventSystem.get());
		sceneManager.loadSceneFromFile("test.scene");
		initSystems();
#if 0
		entt::entity floor = registry.create();
		btCollisionShape* floorShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
		PhysicsComponent& physicsComponent = physicsSystem->createPhysicsBody(floor, floorShape, 0.0f, glm::vec3(0.0f, -0.63f, 0.0f), false);
#endif
#if 0
		createBoxes();
#endif
#if 0
		for (int j = 0; j < 10; j++) {
			for (int i = 0; i < 10; i++) {
				entt::entity entity = registry.create();
				TransformComponent& transform = registry.emplace<TransformComponent>(entity, glm::vec3(2 * j, 2 * i, 0.0f));
				transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
				registry.emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
			}
	}
#endif
#if 0
		createPlayer();

		const std::shared_ptr<ModelImporter> importer = modelLoader->getSuitableImporter("untitled1.gltf");
		const std::shared_ptr<Import::Model> model = importer->import("untitled1.gltf");

		if (model) {
			entt::entity entity = registry.create();
			registry.emplace<TransformComponent>(entity);
			registry.emplace<RenderComponent>(entity, modelLoader->loadFromFile("untitled1.gltf"));
			
			btTriangleMesh* btMesh = new btTriangleMesh();

			for (auto& node : model->nodes) {
				if (node->meshId == -1) continue;

				auto& mesh = model->meshes[node->meshId];

				for (int i = 0; i < mesh->indices.size(); i += 3) {
					if (i + 2 >= mesh->indices.size()) break;

					glm::vec3 pos1 = glm::vec4(mesh->vertices[mesh->indices[i]].pos, 0.0f) * glm::scale(glm::mat4(1.0f), node->scale);
					glm::vec3 pos2 = glm::vec4(mesh->vertices[mesh->indices[i + 1]].pos, 0.0f) * glm::scale(glm::mat4(1.0f), node->scale);
					glm::vec3 pos3 = glm::vec4(mesh->vertices[mesh->indices[i + 2]].pos, 0.0f) * glm::scale(glm::mat4(1.0f), node->scale);

					btMesh->addTriangle(
						btVector3(pos1.x, pos1.y, pos1.z),
						btVector3(pos2.x, pos2.y, pos2.z),
						btVector3(pos3.x, pos3.y, pos3.z)
					);
				}
			}

			btBvhTriangleMeshShape* gTriMeshShape = new btBvhTriangleMeshShape(btMesh, true);
			btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.0f, 0.0f, 0.0)));
			btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, gTriMeshShape);
			btRigidBody* body = new btRigidBody(rbInfo);
			physicsSystem->dynamicsWorld->addRigidBody(body);

			// Specify filters manually, otherwise ghost doesn't collide with statics for some reason
			//physicsSystem->dynamicsWorld->addCollisionObject(m_pGhostObject, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
			PhysicsComponent& physicsComponent = registry.emplace<PhysicsComponent>(entity, gTriMeshShape, motionState, body);
			physicsComponent.isDynamic = true;
			physicsComponent.applyRotation = false;
		}
#endif
	}

	void run() {
        double dt = 0.0;
        
        while (window->isOpen()) {
            m_Start = std::chrono::high_resolution_clock::now();

			eventSystem->flushEvents();

			systemManager.update(dt);
			
			//minterface->update(dt);

			InputManager::instance()->mouseMoveX = 0.0;
			InputManager::instance()->mouseMoveY = 0.0;
			InputManager::instance()->mouseMoveCnt = 0;
			window->pollEvents();

			/*if (window->isActive()) {
				window->setCursorPos(1920.0f / 2.0f, 1080.0f / 2.0f);
				InputManager::instance()->mousePosX = 1920.0f / 2.0f;
				InputManager::instance()->mousePosY = 1080.0f / 2.0f;
			}*/

            dt = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_Start).count();
        }
	}

	void initSystems()
	{
		{
			entt::registry* registry = sceneManager.getCurrentScene()->getRegistry();
			entt::entity entity = registry->create();
			TransformComponent& transform = registry->emplace<TransformComponent>(entity);
			transform.matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			registry->emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
		}
		{
			entt::registry* registry = sceneManager.getCurrentScene()->getRegistry();
			entt::entity entity = registry->create();
			TransformComponent& transform = registry->emplace<TransformComponent>(entity);
			transform.matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			registry->emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
		}
		{
			entt::registry* registry = sceneManager.getCurrentScene()->getRegistry();
			entt::entity entity = registry->create();
			TransformComponent& transform = registry->emplace<TransformComponent>(entity);
			transform.matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			registry->emplace<RenderComponent>(entity, modelLoader->loadFromFile("BoxTextured.gltf"));
		}
	}

	void saveRuntimeDir()
	{
		//Save runtime directory	
		char buffer[1000];
		GetModuleFileName(NULL, buffer, 1000);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		std::string temp = replace(std::string(buffer).substr(0, pos), "\\", "/");

		applicationRegistry->set("runtime_dir", temp);
		applicationRegistry->set("assets_dir", temp);
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