#ifndef VKShader_H_
#define VKShader_H_

#include "VKDevice.h"

#include <string>
#include <vector>

namespace mrhi {

class VKShader : public Shader {
    VKDevice* device;
public:
    ShaderStage stage;

    VkShaderModule shaderModule;

    static VKShader* create(VKDevice* device, const ShaderDesc* d);
    virtual ~VKShader();
private:
};

}
#endif