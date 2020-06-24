#pragma once

#include "LinearMath/btIDebugDraw.h"
#include "Graphics/vertex.h"
#include <vector>
#include <glad/glad.h>

class PhysicsDebugDraw : public btIDebugDraw {
private:
	GLuint vao;
	GLuint vbo;
public:
	PhysicsDebugDraw()
	{
		std::vector<vertex> vertices;
		vertices.push_back(vertex(0.0f, 10.0f, 0.0f, 0.0f, 0.0f));
		vertices.push_back(vertex(0.0f, -10.0f, 0.0f, 0.0f, 1.0f));

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, vertices.size() * sizeof(vertex), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

		glBindVertexArray(0);
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
		glNamedBufferSubData(vbo, 0, 2 * sizeof(vertex), vertices);
		glBindVertexBuffer(0, vbo, 0, sizeof(vertex));
		glDrawArrays(GL_LINES, 0, 2);
	}

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

	virtual void reportErrorWarning(const char* warningString) override {}

	virtual void draw3dText(const btVector3& location, const char* textString) override {}

	virtual void setDebugMode(int debugMode) override { }

	virtual int getDebugMode() const override { return DBG_DrawWireframe | DBG_DrawAabb; }
};