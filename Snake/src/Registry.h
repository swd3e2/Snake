#pragma once

#include "Common/Singleton.h"
#include <unordered_map>
#include <string>

/**
 * Container for application settings
 */
class Registry : public Singleton<Registry> {
private:
	std::unordered_map<std::string, std::string> values;
public:
	std::string get(const std::string& param) {
		if (values.find(param) != values.end()) {
			return values[param];
		}
		return "";
	}
	void set(const std::string& name, const std::string& value) {
		values[name] = value;
	}

	bool has(const std::string& param) {
		return values.find(param) != values.end();
	}
};