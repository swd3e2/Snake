#include "Renderer.h"
#include "Platform/DirectX/DX11Renderer.h"

Renderer* Renderer::_instance = nullptr;
RenderContext* Renderer::_renderContext = nullptr;
MainRenderTarget* Renderer::_mainRenderTarget = nullptr;
RendererType Renderer::_rendererType = RendererType::None;

Renderer* Renderer::create(RendererType type) {
	switch (type) {
		case RendererType::DirectX: return new DX11Renderer();
	}
	return nullptr;
}