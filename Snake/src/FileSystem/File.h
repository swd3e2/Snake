#pragma once

#include <string>
#include <fstream>

class File {
private:
	std::string path;
public:
	File(const std::string path) :
		path(path)
	{
	}

	std::string getConent() {
		std::string content;
		std::ifstream fileStream(path, std::ios::in);

		if (!fileStream.is_open()) {
			return "";
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();

		return content;
	}
};