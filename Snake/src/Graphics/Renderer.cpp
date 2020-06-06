#include "Renderer.h"

Renderer* Renderer::_instance = nullptr;
RenderContext* Renderer::_renderContext = nullptr;
MainRenderTarget* Renderer::_mainRenderTarget = nullptr;
RendererType Renderer::_rendererType = RendererType::None;