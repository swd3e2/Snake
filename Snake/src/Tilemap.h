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
public:
	std::shared_ptr<Texture> texture;
private:
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

	TileTexCoords getTexCoords(int val, bool flipX = false) {
		TileTexCoords texCoords;

		int tileX = val % tilesInRow;
		int tileY = val / tilesInRow;

		double tileLeftCorner = (1.0 / (double)tilesInRow) * (double)tileX;
		double tileRightCorner = (1.0 / (double)tilesInRow) * (tileX + 1.0);
		
		double tileBottomCorner = (1.0 / (double)tilesInColumn) * (double)tileY;
		double tileTopCorner = (1.0 / (double)tilesInColumn) * (tileY + 1.0);

		const double bias = 0.000001;

		if (flipX) {
			texCoords.bottomLeft = { tileRightCorner - bias, 1.0 - tileTopCorner + bias };
			texCoords.bottomRight = { tileLeftCorner + bias, 1.0 - tileTopCorner + bias };
			texCoords.topLeft = { tileRightCorner - bias, 1.0 - tileBottomCorner - bias };
			texCoords.topRight = { tileLeftCorner + bias, 1.0 - tileBottomCorner - bias };
		} else {
			texCoords.bottomLeft = { tileLeftCorner + bias, 1.0 - tileTopCorner + bias };
			texCoords.bottomRight = { tileRightCorner - bias, 1.0 - tileTopCorner + bias };
			texCoords.topLeft = { tileLeftCorner + bias, 1.0 - tileBottomCorner - bias };
			texCoords.topRight = { tileRightCorner - bias, 1.0 - tileBottomCorner - bias };
		}

		return texCoords;
	}
};