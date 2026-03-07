/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef GLShader_H_
#define GLShader_H_

#include "GLDevice.h"

#include <string>
#include <vector>

namespace arhi {

class GLShader : public Shader {
public:
    ShaderStage stage;

    //GLPUShaderModule shaderModule;
    std::string entryPoint = "main";

    GLuint shaderModule;

    static APtr<GLShader> create(GLDevice* device, const ShaderDesc* d);
    virtual ~GLShader();
};

}
#endif