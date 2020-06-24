#pragma once

#include "ImportData.h"

class ModelImporter {
public:
	virtual std::shared_ptr<Import::Model> import(const std::string& filename) = 0;
};