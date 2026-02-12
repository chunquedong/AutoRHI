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

    static WGShader* create(WGDevice* device, const ShaderDesc* d);
    virtual ~WGShader();
};

}
#endif