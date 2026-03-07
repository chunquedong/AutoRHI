/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef WGShader_H_
#define WGShader_H_

#include "WGDevice.h"

#include <string>
#include <vector>

namespace arhi {

class WGShader : public Shader {
public:
    ShaderStage stage;

    WGPUShaderModule shaderModule;

    static APtr<WGShader> create(WGDevice* device, const ShaderDesc* d);
    virtual ~WGShader();
};

}
#endif