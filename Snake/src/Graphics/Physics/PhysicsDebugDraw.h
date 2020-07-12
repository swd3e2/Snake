#pragma once

#include <d3d11.h>
#include "LinearMath/btIDebugDraw.h"
#include "Graphics/vertex.h"
#include <vector>
#include "Graphics/Renderer/VertexBuffer.h"
#include <memory>

class PhysicsDebugDraw : public btIDebugDraw {
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;
public:
	PhysicsDebugDraw()
	{
		std::vector<vertex> vertices;
		vertices.push_back(vertex(0.0f, 10.0f, 0.0f, 0.0f, 0.0f));
		vertices.push_back(vertex(0.0f, -10.0f, 0.0f, 0.0f, 1.0f));

		vertexBuffer.reset(VertexBuffer::create(vertices.size(), sizeof(vertex), vertices.data(), true));
	}

	virtual ~PhysicsDebugDraw() {};

	///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
	virtual void setDefaultColors(const DefaultColors& /*colors*/) {}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		vertex vertices[2] = {
			vertex(from.getX(), from.getY(), from.getZ()),
			vertex(to.getX(),	to.getY(),	 to.getZ())
		};
		vertexBuffer->update(vertices, 2, sizeof(vertex));
		vertexBuffer->bind(Renderer::instance()->getContext());
		Renderer::instance()->draw(2);
	}

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

	virtual void reportErrorWarning(const char* warningString) override {}

	virtual void draw3dText(const btVector3& location, const char* textString) override {}

	virtual void setDebugMode(int debugMode) override { }

	virtual int getDebugMode() const override { return DBG_DrawWireframe | DBG_DrawAabb; }
};