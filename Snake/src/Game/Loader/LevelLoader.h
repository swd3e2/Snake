#pragma once

#include "../RawLevel.h"
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <string>

class LevelLoader {
public:
	std::shared_ptr<RawLevel> loadFromFile(const std::string& path) {
        std::shared_ptr<RawLevel> level = std::make_shared<RawLevel>();

        std::ifstream myfile(path);

        std::string line;
        while (std::getline(myfile, line)) {
            std::vector<int> currentLineVector;
            std::string temp;

            for (auto& it : line) {
                if (it != ',') {
                    temp += it;
                } else {
                    currentLineVector.push_back(std::stoi(temp));
                    temp.clear();
                }
            }
            level->map.push_back(currentLineVector);
        }
        return level;
	}
};