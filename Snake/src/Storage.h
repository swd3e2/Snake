#pragma once

#include "Graphics/Renderer/Texture2D.h"
#include "Model/Model.h"
#include <memory>
#include <unordered_map>
#include <string>
#include "Singleton.h"

template<typename T>
class Storage : public Singleton<Storage<T>> {
private:
	std::unordered_map<std::string, std::weak_ptr<T>> storage;
public:
	bool has(const std::string& name) {
		if (storage.find(name) != storage.end()) {
			return !storage[name].expired();
		}
		return false;
	}

	std::weak_ptr<T> get(const std::string& name) {
		return storage[name];
	}

	void put(const std::string& name, T* object) {
		storage[name] = object;
	}
};
