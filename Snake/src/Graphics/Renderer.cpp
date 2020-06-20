#include "Renderer.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

Renderer* Renderer::_instance = nullptr;
RenderContext* Renderer::_renderContext = nullptr;
MainRenderTarget* Renderer::_mainRenderTarget = nullptr;
RendererType Renderer::_rendererType = RendererType::None;

Renderer* Renderer::create(RendererType type) {
	switch (type) {
		case RendererType::OpenGL: return new OpenGlRenderer();
	}
	return nullptr;
}