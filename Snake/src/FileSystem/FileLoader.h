#pragma once

#include <string>

class FileLoader {
public:
	virtual std::string getFileContent(const std::string& path) = 0;
};