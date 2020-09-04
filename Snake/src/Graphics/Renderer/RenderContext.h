#pragma once

class ShaderPipeline;
class DX11Renderer;

class RenderContext 
{
private:
	friend class DX11Renderer;
	size_t drawCalls = 0;
public:
	virtual ShaderPipeline* getCurrentShader() = 0;
	size_t getDrawCallCount() const { return drawCalls; }
};