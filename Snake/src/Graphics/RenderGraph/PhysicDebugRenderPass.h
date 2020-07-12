#pragma once

#include "Pass.h"
#include <glm/glm.hpp>
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/vertex.h"
#include <memory>
#include "Physics/PhysicsSystem.h"

struct Line {
	glm::vec3 from;
	glm::vec3 to;
};

class PhysicDebugRenderPass : public Pass {
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;
	vertex* lines = nullptr;
	int linesSize = 0;
	PhysicsSystem* phsyicsSystem = nullptr;
public:
	PhysicDebugRenderPass(PhysicsSystem* phsyicsSystem, const std::string& name) :
		phsyicsSystem(phsyicsSystem), Pass(name)
	{
		//lines = (vertex*)malloc(sizeof(Line) * 10000);

		std::vector<vertex> vertices;
		vertices.push_back(vertex(0.0f, 10.0f, 0.0f, 0.0f, 0.0f));
		vertices.push_back(vertex(0.0f, -10.0f, 0.0f, 0.0f, 1.0f));

		vertexBuffer.reset(VertexBuffer::create(vertices.size(), sizeof(vertex), vertices.data(), true));
	}

	virtual void execute(Renderer* renderer) {
		renderer->setViewport(viewport);
		renderTarget->bind(renderer->getContext());

		for (auto& it : textures) {
			it.second->bindToUnit(it.first, renderer->getContext());
		}

		this->shader->bind(renderer->getContext());

		Renderer::instance()->setPrimitiveTopology(PrimitiveTopology::LINELIST);
		phsyicsSystem->dynamicsWorld->debugDrawWorld();
		Renderer::instance()->setPrimitiveTopology(PrimitiveTopology::TRIANGELIST);

		/*if (linesSize > 0) {
			vertexBuffer->update(lines, linesSize * 2, sizeof(vertex));
			vertexBuffer->bind(renderer->getContext());

			renderer->draw(linesSize * 2);
		}

		linesSize = 0;*/
	}

	void addLine(glm::vec3& from, glm::vec3& to) {
		vertex* vertex = lines + linesSize;
		vertex->pos = from;

		vertex++;
		vertex->pos = to;

		linesSize++;
	}
};