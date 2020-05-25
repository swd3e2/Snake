#pragma once

#include "../RawLevel.h"
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "../../FileSystem/File.h"

class LevelLoader {
public:
	std::shared_ptr<RawLevel> loadFromFile(const std::string& path) {
        std::shared_ptr<RawLevel> level = std::make_shared<RawLevel>();

        std::ifstream myfile(path);

        using json = nlohmann::json;

        File levelFile("test.json");

        auto levelData = json::parse(levelFile.getConent());

        std::map<int, std::map<int, TileData>> map;
        for (auto& it : levelData["layers"][0]["tiles"]) {
            TileData data;
            data.tileIndex = it["tile"].get<int>();
            data.flipX = it["flipX"].get<bool>();

            map[it["y"].get<int>()][it["x"].get<int>()] = data;
        }

        for (auto& it : map) {
            std::vector<TileData> currentLineVector;
            for (auto& iit : it.second) {
                currentLineVector.push_back(iit.second);
            }
            level->map.push_back(currentLineVector);
        }

        return level;
	}
};