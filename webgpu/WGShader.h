#ifndef WGShader_H_
#define WGShader_H_

#include "WGDevice.h"

#include <string>
#include <vector>

namespace mrhi {

class WGShader : public Shader {
public:
    ShaderStage stage;

    WGPUShaderModule shaderModule;

    static WGShader* create(WGDevice* device, const ShaderDesc* d);
    virtual ~WGShader();
};

}
#endif