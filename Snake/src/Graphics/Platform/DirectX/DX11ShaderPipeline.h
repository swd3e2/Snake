#pragma once

#include "Graphics/Renderer/ShaderPipeline.h"
#include "DX11Renderer.h"
#include <d3dcompiler.h>
#include "DX11Utils.h"
#include "Graphics/Renderer/CommonTypes.h"

class DX11ShaderPipeline : public ShaderPipeline {
private:
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
public:
	DX11ShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode) :
		ShaderPipeline(vertexShaderCode, pixelShaderCode)
	{
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3DBlob* shaderBlob = nullptr;

		if (compileShaderInternal(vertexShaderCode, ShaderType::VERTEX, &shaderBlob)) {
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &vertexShader);
			shaderBlob->Release();
		} else {
			std::cout << "Failed to compile vertex shader" << std::endl;
		}

		if (compileShaderInternal(pixelShaderCode, ShaderType::PIXEL, &shaderBlob)) {
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pixelShader);
			shaderBlob->Release();
		} else {
			std::cout << "Failed to compile pixel shader" << std::endl;
		}
	}

	bool compileShaderInternal(const std::string& code, ShaderType type, ID3DBlob** blob) {
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		const D3D_SHADER_MACRO defines[] = {
			{ "HLSL", "1" },
			{ nullptr, nullptr }
		};

		HRESULT hr = D3DCompile(code.c_str(), code.size(), NULL, defines, NULL,
			"main", DirectX::getShaderTarget(type).c_str(), D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0, &shaderBlob, &errorBlob);

		if (FAILED(hr)) {
			if (errorBlob) {
				std::cout << ((const char*)errorBlob->GetBufferPointer()) << std::endl;
				errorBlob->Release();
			}
			if (shaderBlob) {
				shaderBlob->Release();
			}

			return false;
		}

		*blob = shaderBlob;
		return true;
	}

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
	}
};