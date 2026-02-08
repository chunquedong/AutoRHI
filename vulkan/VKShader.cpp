#include "VKShader.h"

#include "spirv/rhi_spirv.h"

using namespace mrhi;


VKShader* VKShader::create(VKDevice* device, const ShaderDesc* d) {

    const char* source = d->source;
    ShaderStage stage = d->stage;
    ShaderLanguage language = d->language;

    VkShaderModule shaderModule = nullptr;
    ShaderModuleReflection reflection;
    if (language == ShaderLanguage::GLSL) {
        std::vector<uint32_t> spirvSource;
        compileGLSL(source, stage, spirvSource);

        spirvReflect(spirvSource, reflection);

        VkShaderModuleCreateInfo module_info{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = spirvSource.size() * sizeof(uint32_t),
            .pCode = spirvSource.data() };

        VK_CHECK(vkCreateShaderModule(device->device, &module_info, nullptr, &shaderModule));
    }
    else {
        MGP_ERROR("Unsupport Shader Language %d\n", language);
    }

    if (!shaderModule) {
        return nullptr;
    }

    VKShader* shader = new VKShader();
    shader->stage = d->stage;
    shader->device = device;
    shader->reflection = std::move(reflection);
    shader->shaderModule = shaderModule;

    return shader;
}

VKShader::~VKShader() {
    vkDestroyShaderModule(device->device, shaderModule, nullptr);
}