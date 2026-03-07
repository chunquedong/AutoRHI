/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef VKShader_H_
#define VKShader_H_

#include "VKDevice.h"

#include <string>
#include <vector>

namespace arhi {

class VKShader : public Shader {
    VKDevice* device;
public:
    ShaderStage stage;

    VkShaderModule shaderModule;

    static APtr<VKShader> create(VKDevice* device, const ShaderDesc* d);
    virtual ~VKShader();
private:
};

}
#endif