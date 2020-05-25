#pragma once

#include "RawLevel.h"
#include "Level.h"
#include <memory>
#include "../Graphics/vertex.h"
#include "../Tilemap.h"



class LevelBuilder {
public:
	std::shared_ptr<Level> build(const std::shared_ptr<RawLevel>& rawLevel, const std::shared_ptr<Tilemap>& tilemap) {
		std::shared_ptr<Level> level = std::make_shared<Level>();

		std::vector<vertex> vertices;
		std::vector<unsigned int> indices;

		for (int y = 0; y < rawLevel->map.size(); y++) {
			for (int x = 0; x < rawLevel->map[y].size(); x++) {
				TileTexCoords texCoords = tilemap->getTexCoords(rawLevel->map[y][x].tileIndex, rawLevel->map[y][x].flipX);

				const int leftBottomIndex = vertices.size();
				vertices.push_back(vertex(x,	  -y,		0.0f, 1.0f, texCoords.bottomLeft.x, texCoords.bottomLeft.y));

				const int rightBottomIndex = vertices.size();
				vertices.push_back(vertex(x + 1,  -y,		0.0f, 1.0f, texCoords.bottomRight.x, texCoords.bottomRight.y));

				const int leftTopIndex = vertices.size();
				vertices.push_back(vertex(x,	  -y + 1,	0.0f, 1.0f, texCoords.topLeft.x, texCoords.topLeft.y));

				const int rightTopIndex = vertices.size();
				vertices.push_back(vertex(x + 1,  -y + 1,	0.0f, 1.0f, texCoords.topRight.x, texCoords.topRight.y));

				indices.push_back(leftTopIndex);
				indices.push_back(rightTopIndex);
				indices.push_back(leftBottomIndex);

				indices.push_back(leftBottomIndex);
				indices.push_back(rightTopIndex);
				indices.push_back(rightBottomIndex);
			}
		}

		level->vertexBuffer = std::make_shared<VertexBuffer>(vertices.size(), sizeof(vertex), vertices.data());
		level->indexBuffer = std::make_shared<IndexBuffer>(indices.size(), indices.data());
		level->texture = tilemap->texture;

		return level;
	}
};