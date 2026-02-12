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

    static GLShader* create(GLDevice* device, const ShaderDesc* d);
    virtual ~GLShader();
};

}
#endif