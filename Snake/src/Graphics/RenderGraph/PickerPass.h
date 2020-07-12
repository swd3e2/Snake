#pragma once

#include "Pass.h"
#include "Graphics/Renderer/ConstantBuffer.h"

class PickerPass : public Pass {
private:
	std::shared_ptr<ConstantBuffer> cbuffer;
public:
	PickerPass(const std::string& name) :
		Pass(name)
	{
		cbuffer.reset(ConstantBuffer::create(1, sizeof(int), nullptr));
	}
};