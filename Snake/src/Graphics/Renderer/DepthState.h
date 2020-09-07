#pragma once

#include "CommonTypes.h"

class RenderContext;

struct DepthStencilOpStateDesc 
{
	StencilOperation failOp;
	StencilOperation depthFailOp;
	StencilOperation passOp;
	ComparisonFunction stencilFunc;
};

struct DepthStateDesc
{
	bool						depthEnable;
	ComparisonFunction			depthFunc;
	DepthWriteMask				depthWriteMask;
	bool						stencilEnable;
	unsigned char				stencilWriteMask;
	unsigned char				stencilReadMask;
	DepthStencilOpStateDesc		front;
	DepthStencilOpStateDesc		back;

	DepthStateDesc() :
		depthEnable(true), depthFunc(ComparisonFunction::LESS), depthWriteMask(DepthWriteMask::ALL), 
		stencilEnable(false), stencilWriteMask(0xFF), stencilReadMask(0xFF)
	{}
};

class DepthState
{
public:
	DepthStateDesc desc;
public:
	DepthState(const DepthStateDesc& desc) : desc(desc) { }
	static DepthState* create(const DepthStateDesc& desc);
	
	virtual void bind(RenderContext* context) = 0;
};