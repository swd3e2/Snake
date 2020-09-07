#pragma once

#include "CommonTypes.h"

class RenderContext;

struct RenderTargetBlendState
{
	bool			blendEnabled	= false;
	BlendFunction	srcBlend		= BlendFunction::ONE;
	BlendFunction	srcBlendAlpha	= BlendFunction::ONE;
	BlendFunction	dstBlend		= BlendFunction::ZERO;
	BlendFunction	dstBlendAlpha	= BlendFunction::ZERO;
	BlendOperation	blendOp			= BlendOperation::ADD;
	BlendOperation	blendOpAlpha	= BlendOperation::ADD;

	unsigned char	writeMask;
};

struct BlendStateDesc
{
	bool					alphaToCoverageEnable = false;
	bool					independentBlendEnable = false;
	RenderTargetBlendState	blend[8];
	int						count = 0;
};

class BlendState
{
private:
	BlendStateDesc desc;
public:
	BlendState(const BlendStateDesc& desc) : desc(desc) {}
	static BlendState* create(const BlendStateDesc& state);

	virtual void bind(RenderContext* context) = 0;
};