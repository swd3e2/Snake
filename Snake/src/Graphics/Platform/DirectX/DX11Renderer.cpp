#include "DX11Renderer.h"

DX11Renderer::DX11Renderer()
{
	_rendererType = RendererType::DirectX;
}

DX11Renderer::~DX11Renderer()
{

}

Window* DX11Renderer::createWindow(int width, int height)
{
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	window.reset(new DX11Window(width, height));
	DX11MainRenderTarget* mainRenderTarget = new DX11MainRenderTarget(width, height);
	_mainRenderTarget = mainRenderTarget;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.OutputWindow = (window->getHwnd());

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_1;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

	HRESULT hr = S_OK;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags,
		featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
		&device, &m_featureLevel, &deviceContext);

	_renderContext = new DX11RenderContext(device, deviceContext, swapChain);

	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mainRenderTarget->m_BackBuffer);
	hr = device->CreateRenderTargetView(mainRenderTarget->m_BackBuffer, NULL, &mainRenderTarget->m_RenderTarget);

	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	desc.Width = width;

	hr = device->CreateTexture2D(&desc, NULL, &mainRenderTarget->m_DepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC ddesc = {};
	ddesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ddesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(mainRenderTarget->m_DepthStencilBuffer, &ddesc, &mainRenderTarget->m_DepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(mainRenderTarget->m_DepthStencilBuffer, &srvd, &mainRenderTarget->m_DepthResourceView);

	deviceContext->RSSetViewports(1, &vp);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_RASTERIZER_DESC rasterizerDesc{ };
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);

	deviceContext->RSSetState(m_RasterizerState);

	return window.get();
}

void DX11Renderer::swapBuffers()
{
	_renderContext->drawCalls = 0;
	swapChain->Present(false, 0);
}

void DX11Renderer::drawIndexed(int cnt)
{
	_renderContext->drawCalls++;
	deviceContext->DrawIndexed(cnt, 0, 0);
}

void DX11Renderer::draw(int cnt)
{
	_renderContext->drawCalls++;
	deviceContext->Draw(cnt, 0);
}

void DX11Renderer::setPrimitiveTopology(PrimitiveTopology topology)
{
	deviceContext->IASetPrimitiveTopology(DirectX::getTopology(topology));
}

void DX11Renderer::setViewport(int x0, int y0, int x1, int y1)
{
	vp.Width = (float)x1;
	vp.Height = (float)y1;
	vp.TopLeftX = x0;
	vp.TopLeftY = y0;

	deviceContext->RSSetViewports(1, &vp);
}

void DX11Renderer::setViewport(const Viewport& viewport)
{
	setViewport(viewport.x0, viewport.y0, viewport.x1, viewport.y1);
}

void DX11Renderer::setDefaultDepthStencil()
{
	deviceContext->OMSetDepthStencilState(nullptr, 0xFF);
}

void DX11Renderer::setDefaultBlendState()
{
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void DX11Renderer::unbindResource(int slot)
{
	deviceContext->PSSetShaderResources(slot, 1, nullShaderResourceView);
}
