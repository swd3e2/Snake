#pragma once

#include "Scene.h"
#include <vector>
#include "SceneLoader.h"
#include "SceneSaver.h"
#include "JobSystem.h"
#include "Common/Helper.h"

class SceneManager 
{
private:
	SceneLoader loader;
	SceneSaver saver;

	std::vector<Scene*> scenes;
	Scene* currentScene = nullptr;
public:
	inline Scene* getCurrentScene() 
	{
		if (currentScene == nullptr) {
			currentScene = new Scene();
		}
		return currentScene;
	}

	void loadSceneFromFile(const std::string& filename)
	{
		std::string name = filename.substr(0, filename.find('.'));
		Scene* newScene = new Scene();
		loader.loadFromFile(filename, newScene->getRegistry());
		scenes.push_back(newScene);
	}

	void saveCurrentScene()
	{
		if (currentScene != nullptr) {
			saver.saveToFile(currentScene->getName() + ".scene", currentScene->getRegistry());
		}
	}
};