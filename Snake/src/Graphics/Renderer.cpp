#include "Renderer.h"

Renderer* Renderer::_instance = nullptr;
RenderContext* Renderer::_renderContext = nullptr;
RendererType Renderer::_rendererType = RendererType::None;