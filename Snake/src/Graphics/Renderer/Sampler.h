#pragma once

#include "Graphics/Bindable.h"

class Sampler : public Bindable {
public:
    static Sampler* create();
};