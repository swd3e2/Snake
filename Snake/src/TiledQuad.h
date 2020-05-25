#pragma once

#include "Graphics/Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/vertex.h"

class TiledQuad {
public:
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<Texture> texture;
public:
	TiledQuad(std::shared_ptr<Texture> texture, glm::vec2 tile, glm::vec2 tileSize) :
		texture(texture) 
	{
		std::vector<vertex> data;

		float tileWidth = texture->getWidth() / tileSize.x;
		float tileHeight= texture->getHeight() / tileSize.y;

		double tileWidthMinRelative = (1.0f / tileWidth) * tile.x;
		double tileHeightMinRelative = (1.0f / tileHeight) * tile.y;
		double tileWidthMaxRelative = (1.0f / tileWidth) * (tile.x + 1);
		double tileHeightMaxRelative = (1.0f / tileHeight) * (tile.y + 1);

		data.push_back(vertex(1.0f, 1.0f, 0.0f, 1.0f, tileWidthMaxRelative, tileHeightMaxRelative));
		data.push_back(vertex(1.0f, 0.0f, 0.0f, 1.0f, tileWidthMaxRelative, tileHeightMinRelative));
		data.push_back(vertex(0.0f, 0.0f, 0.0f, 1.0f, tileWidthMinRelative, tileHeightMinRelative));
		data.push_back(vertex(0.0f, 1.0f, 0.0f, 1.0f, tileWidthMinRelative, tileHeightMaxRelative));

		vertexBuffer = std::make_shared<VertexBuffer>(4, sizeof(vertex), data.data());

		std::vector<unsigned int> idata;
		idata.push_back(0);
		idata.push_back(1);
		idata.push_back(2);
		idata.push_back(2);
		idata.push_back(3);
		idata.push_back(0);

		indexBuffer = std::make_shared<IndexBuffer>(idata.size(), idata.data());
	}
};