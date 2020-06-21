#pragma once

#include "Graphics/Bindable.h"
#include "Graphics/Renderer/CommonTypes.h"

#define MAX_LOD (3.402823466e+38f)

struct SamplerDesc {
	AddressingMode addressingMode;
	FilterMode minFilterModel;
	FilterMode magFilterMode;
	FilterMode mipFilterMode;
	int maxAnisotropy;
	ComparisonFunction comparisonFunction;
	float minLOD;
	float maxLOD;
	float mipLODBias;

	SamplerDesc() :
		addressingMode(AddressingMode::WRAP), minFilterModel(FilterMode::LINEAR), magFilterMode(FilterMode::LINEAR),
		mipFilterMode(FilterMode::NONE), maxAnisotropy(1), comparisonFunction(ComparisonFunction::NEVER), minLOD(0),
		maxLOD(MAX_LOD), mipLODBias(0)
	{}
};

class Sampler : public Bindable {
private:
	AddressingMode addressingMode = AddressingMode::WRAP;
	FilterMode minFilterModel = FilterMode::POINT;
	FilterMode magFilterMode = FilterMode::POINT;
	FilterMode mipFilterMode = FilterMode::NONE;
	int maxAnisotropy = 1;
	ComparisonFunction comparisonFunction = ComparisonFunction::NEVER;
	float minLOD = 0;
	float maxLOD = MAX_LOD;
	float mipLODBias = 0;
protected:
	unsigned int textureUnit = 0;
	unsigned int samplerUnit = 0;
public:
	Sampler(AddressingMode addressingMode, FilterMode minFilterModel, FilterMode magFilterMode, FilterMode mipFilterMode, int maxAnisotropy, ComparisonFunction comparisonFunction, float minLOD, float maxLOD, float mipLODBias) :
		addressingMode(addressingMode), minFilterModel(minFilterModel), magFilterMode(magFilterMode), mipFilterMode(mipFilterMode), maxAnisotropy(maxAnisotropy), comparisonFunction(comparisonFunction), minLOD(minLOD), maxLOD(maxLOD), mipLODBias(mipLODBias)
	{}

	static Sampler* create(AddressingMode addressingMode, FilterMode minFilterModel, FilterMode magFilterMode, FilterMode mipFilterMode, int maxAnisotropy, ComparisonFunction comparisonFunction, float minLOD, float maxLOD, float mipLODBias);
	static Sampler* create(const SamplerDesc& desc);

	void setTextureUnit(unsigned int textureUnit) {
		this->textureUnit = textureUnit;
	}

	virtual void setSamplerUnit(unsigned int samplerUnit) {
		this->samplerUnit = samplerUnit;
	}
};