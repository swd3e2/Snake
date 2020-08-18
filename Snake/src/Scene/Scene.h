#pragma once

#include <entt/entt.hpp>
#include <string>

class Scene 
{
private:
	entt::registry registry;
	std::string name;
public:
	inline entt::registry* getRegistry() { return &registry; }
	inline std::string getName() const { return name; }
};