#include "Sampler.h"
#include "Graphics/Platform/DirectX/DX11Sampler.h"
#include "Graphics/Renderer.h"

Sampler* Sampler::create(AddressingMode addressingMode, FilterMode minFilterModel, FilterMode magFilterMode, FilterMode mipFilterMode, int maxAnisotropy, ComparisonFunction comparisonFunction, float minLOD, float maxLOD, float mipLODBias) {
	switch (Renderer::getType()) {
		case RendererType::DirectX: return new DX11Sampler(addressingMode, minFilterModel, magFilterMode, mipFilterMode, maxAnisotropy, comparisonFunction, minLOD, maxLOD, mipLODBias);
	}
	return nullptr;
}

Sampler* Sampler::create(const SamplerDesc& desc) {
	return create(desc.addressingMode, desc.minFilterModel, desc.magFilterMode, desc.mipFilterMode, desc.maxAnisotropy, desc.comparisonFunction, desc.minLOD, desc.maxLOD, desc.mipLODBias);
}