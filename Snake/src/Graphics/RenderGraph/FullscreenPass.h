#pragma once

#include "Pass.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/IndexBuffer.h"
#include "Graphics/vertex.h"
#include <vector>

/**
 * Class for post processing effects.
 * It renders a single quad.
 */
class FullscreenPass : public Pass {
private:
    std::shared_ptr<VertexBuffer> quadVertexBuffer;
    std::shared_ptr<IndexBuffer> quadIndexBuffer;
public:
    FullscreenPass(const std::string& name) :
        Pass(name) 
    {
        std::vector<vertex> vertexData;
        vertexData.push_back(vertex( 1.0f,  1.0f,  0.1f, 1.0f, 0.0f));
        vertexData.push_back(vertex( 1.0f, -1.0f,  0.1f, 1.0f, 1.0f));
        vertexData.push_back(vertex(-1.0f,  1.0f,  0.1f, 0.0f, 0.0f));
        vertexData.push_back(vertex(-1.0f, -1.0f,  0.1f, 0.0f, 1.0f));
        quadVertexBuffer.reset(VertexBuffer::create(vertexData.size(), sizeof(vertex), vertexData.data()));

        std::vector<unsigned int> indexData;
		indexData.push_back(0); indexData.push_back(2); indexData.push_back(1);
		indexData.push_back(2); indexData.push_back(3); indexData.push_back(1);
        quadIndexBuffer.reset(IndexBuffer::create(indexData.size(), indexData.data()));
    }

    virtual void execute(Renderer* renderer) {
        renderer->setViewport(viewport);
        renderTarget->bind(renderer->getContext());

		for (auto& it : textures) {
			it.second->bindToUnit(it.first, renderer->getContext());
		}
        for (auto& it : buffers) {
            it->bind(renderer->getContext());
        }
		this->shader->bind(renderer->getContext());

        quadVertexBuffer->bind(renderer->getContext());
        quadIndexBuffer->bind(renderer->getContext());

        renderer->drawIndexed(quadIndexBuffer->getSize());
    }
};