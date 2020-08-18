#pragma once

#include "Scene/SceneManager.h"

class ISystem {
protected:
	SceneManager* sceneManager = nullptr;
public:
	ISystem(SceneManager* sceneManager) :
		sceneManager(sceneManager)
	{}

	virtual void update(double dt) = 0;
};