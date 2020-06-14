#pragma once

#include "Graphics/Renderer/Sampler.h"
#include "OpenGLUtils.h"
#include <glad/glad.h>

class OpenGLSampler : public Sampler {
private:
	GLuint samplerId;
public:
	OpenGLSampler(AddressingMode addressingMode, FilterMode minFilterModel, FilterMode magFilterMode, FilterMode mipFilterMode, int maxAnisotropy, ComparisonFunction comparisonFunction, float minLOD, float maxLOD, float mipLODBias) : 
		Sampler(addressingMode, minFilterModel, magFilterMode, mipFilterMode, maxAnisotropy, comparisonFunction, minLOD, maxLOD, mipLODBias) 
	{
		glGenSamplers(1, &samplerId);

		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, getAddressingMode(addressingMode));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, getAddressingMode(addressingMode));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_R, getAddressingMode(addressingMode));

		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, getFilteringMode(magFilterMode));
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, getMinMipFilteringMode(minFilterModel, mipFilterMode));

		glSamplerParameterf(samplerId, GL_TEXTURE_MIN_LOD, minLOD);
		glSamplerParameterf(samplerId, GL_TEXTURE_MAX_LOD, maxLOD);
		glSamplerParameterf(samplerId, GL_TEXTURE_LOD_BIAS, mipLODBias);
		glSamplerParameterf(samplerId, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);

		if (comparisonFunction != ComparisonFunction::NEVER) {
			glSamplerParameteri(samplerId, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glSamplerParameteri(samplerId, GL_TEXTURE_COMPARE_FUNC, getComparisonFunction(comparisonFunction));
		} else {
			glSamplerParameteri(samplerId, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		}
	}

	virtual void bind(RenderContext* renderContext) override {
		glBindSampler(textureUnit, samplerId);
	}
};