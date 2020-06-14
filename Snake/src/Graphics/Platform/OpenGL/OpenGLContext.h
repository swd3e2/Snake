#pragma once

#include "Graphics/Renderer/RenderContext.h"
#include <glm/glm.hpp>
#include "Graphics/Renderer/CommonTypes.h"
#include <vector>

class OpenGLContext : public RenderContext
{
private:
    glm::vec4 viewport;
    std::vector<int> activatedTextureUnits;
	bool cullEnabled = false;
	bool depthEnabled = false;
	bool stencilEnabled = false;
	bool frontCounterClockwise = false;
	FillMode fillMode = FillMode::SOLID;
	CullMode cullMode = CullMode::NONE;
public:
    OpenGLContext() {
        
    }
};