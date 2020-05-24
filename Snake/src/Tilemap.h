#pragma once

#include "Graphics/Texture.h"
#include <memory>
#include <glm/glm.hpp>
#include "Game/RawLevel.h"

struct TileTexCoords {
	glm::vec2 bottomLeft;
	glm::vec2 bottomRight;
	glm::vec2 topLeft;
	glm::vec2 topRight;
};

class Tilemap {
private:
	std::shared_ptr<Texture> texture;
	glm::vec2 tileSize;
	int tilesInRow;
	int tilesInColumn;
public:
	Tilemap(const std::shared_ptr<Texture>& texture, glm::vec2 tileSize) :
		texture(texture), tileSize(tileSize)
	{
		tilesInRow = texture->getWidth() / tileSize.x;
		tilesInColumn = texture->getHeight() / tileSize.y;
	}

	TileTexCoords getTexCoords(int val) {
		TileTexCoords texCoords;

		int tileX = val % tilesInRow;
		int tileY = val / tilesInRow;

		double tileLeftCorner = (1.0 / (double)tilesInRow) * (double)tileX;
		double tileBottomCorner = (1.0 / (double)tilesInColumn) * (double)tileY;
		double tileRightCorner = (1.0 / (double)tilesInRow) * (tileX + 1.0);
		double tileTopCorner = (1.0 / (double)tilesInColumn) * (tileY + 1.0);

		if (0) {
			texCoords.bottomLeft = { tileLeftCorner, tileBottomCorner };
			texCoords.bottomRight = { tileRightCorner, tileBottomCorner };
			texCoords.topLeft = { tileLeftCorner, tileTopCorner };
			texCoords.topRight = { tileRightCorner, tileTopCorner };
		} else {
			const double bias = 0.000001;
			texCoords.bottomLeft = { tileLeftCorner + bias, 1.0 - tileTopCorner + bias };
			texCoords.bottomRight = { tileRightCorner - bias, 1.0 - tileTopCorner + bias };
			texCoords.topLeft = { tileLeftCorner + bias, 1.0 - tileBottomCorner - bias };
			texCoords.topRight = { tileRightCorner - bias, 1.0 - tileBottomCorner - bias };
		}

		return texCoords;
	}
};