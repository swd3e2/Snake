#pragma once

#include <string>
#include "CommonTypes.h"
#include "Graphics/Bindable.h"
#include <vector>

struct ShaderInputLayoutDesc {
	struct InputLayoutElement {
		InputDataType type;
		std::string name;
	};
	std::vector<InputLayoutElement> elements;
};

class ShaderInputLayout : public Bindable {
public:
	static ShaderInputLayout* create(const ShaderInputLayoutDesc& desc);
};