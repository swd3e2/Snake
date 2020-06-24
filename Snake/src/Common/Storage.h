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

	std::shared_ptr<T> get(const std::string& name) {
		if (storage.find(name) != storage.end()) {
			return storage[name].lock();
		}
		return nullptr;
	}

	void set(const std::string& name, const std::shared_ptr<T>& object) {
		storage[name] = object;
	}
};
