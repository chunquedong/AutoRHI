#include "WGShader.h"

#include "spirv/rhi_spirv.h"

using namespace arhi;

static void reflectionCallback(WGPUReflectionInfoRequestStatus status, WGPUReflectionInfo const* reflectionInfo, void* userdata1, void* userdata2) {
    WGShader* shader = (WGShader*)userdata1;

    for (uint32_t i = 0; i < reflectionInfo->globalCount; i++) {
        UniformVar uniform;
        uniform.type = UniformType::Unknow;
        const char* typedesc = NULL;

        if (reflectionInfo->globals[i].buffer.type != WGPUBufferBindingType_BindingNotUsed) {
            typedesc = "buffer";
            uniform.type = UniformType::UniformBuffer;
            uniform.bufferBindingType = reflectionInfo->globals[i].buffer.type;
            uniform.bufferMinBindingSize = reflectionInfo->globals[i].buffer.minBindingSize;
        }
        else if (reflectionInfo->globals[i].texture.sampleType != WGPUTextureSampleType_BindingNotUsed) {
            typedesc = "texture";
            uniform.type = UniformType::Texture;
            uniform.textureSampleType = reflectionInfo->globals[i].texture.sampleType;
            uniform.textureViewDimension = reflectionInfo->globals[i].texture.viewDimension;
        }
        else if (reflectionInfo->globals[i].sampler.type != WGPUSamplerBindingType_BindingNotUsed) {
            typedesc = "sampler";
            uniform.type = UniformType::Sampler;
            uniform.samplerBindingType = reflectionInfo->globals[i].sampler.type;
        }
        else {
            MGP_ERROR("Unsupport reflect uniform type\n");
            continue;
        }

#if 0
        char namebuffer[256] = { 0 };

        const WGPUGlobalReflectionInfo* toBePrinted = reflectionInfo->globals + i;

        memcpy(namebuffer, toBePrinted->name.data, toBePrinted->name.length);
        MGP_ERROR("Name: %s, location: %u, type: %s", namebuffer, toBePrinted->binding, typedesc);
        if (reflectionInfo->globals[i].buffer.type != WGPUBufferBindingType_BindingNotUsed) {
            MGP_ERROR(", minBindingSize = %d", (int)toBePrinted->buffer.minBindingSize);
        }
        MGP_ERROR("\n");
#endif

        const WGPUGlobalReflectionInfo* info = reflectionInfo->globals + i;
        
        uniform.visibility = (int)shader->stage;
        uniform.name = std::string(info->name.data, info->name.length);
        uniform.binding = info->binding;
        uniform.bindGroup = info->bindGroup;
        shader->reflection.uniforms.push_back(uniform);
    }
    if (reflectionInfo->inputAttributes) {
        for (uint32_t i = 0; i < reflectionInfo->inputAttributes->attributeCount; i++) {
            WGPUReflectionAttribute* info = reflectionInfo->inputAttributes->attributes + i;
            AttributeVar attri;
            //attri.name = std::string(info->name.data, info->name.length);
            attri.location = info->location;
            attri.format = 0;
            if (info->componentType == WGPUReflectionComponentType_Float32) {
                if (info->compositionType == WGPUReflectionCompositionType_Scalar) {
                    attri.format = WGPUVertexFormat_Float32;
                }
                if (info->compositionType == WGPUReflectionCompositionType_Vec2) {
                    attri.format = WGPUVertexFormat_Float32x2;
                }
                else if (info->compositionType == WGPUReflectionCompositionType_Vec3) {
                    attri.format = WGPUVertexFormat_Float32x3;
                }
                else if (info->compositionType == WGPUReflectionCompositionType_Vec4) {
                    attri.format = WGPUVertexFormat_Float32x4;
                }
            }
            //MGP_ERROR("input attribute: %d, %d\n", info->location, attri.format);
            shader->reflection.inputAttrs.push_back(attri);
        }
    }
    if (reflectionInfo->outputAttributes) {
        for (uint32_t i = 0; i < reflectionInfo->outputAttributes->attributeCount; i++) {
            WGPUReflectionAttribute* info = reflectionInfo->outputAttributes->attributes + i;
            AttributeVar attri;
            //attri.name = std::string(info->name.data, info->name.length);
            attri.location = info->location;
            attri.format = 0;
            if (info->componentType == WGPUReflectionComponentType_Float32) {
                if (info->compositionType == WGPUReflectionCompositionType_Scalar) {
                    attri.format = WGPUVertexFormat_Float32;
                }
                if (info->compositionType == WGPUReflectionCompositionType_Vec2) {
                    attri.format = WGPUVertexFormat_Float32x2;
                }
                else if (info->compositionType == WGPUReflectionCompositionType_Vec3) {
                    attri.format = WGPUVertexFormat_Float32x3;
                }
                else if (info->compositionType == WGPUReflectionCompositionType_Vec4) {
                    attri.format = WGPUVertexFormat_Float32x4;
                }
            }
            //MGP_ERROR("output attribute: %d, %d\n", info->location, attri.format);
            shader->reflection.outputAttrs.push_back(attri);
        }
    }
}


WGShader* WGShader::create(WGDevice* device, const ShaderDesc* d) {

    const char* source = d->source;
    WGPUShaderStage stage = (WGPUShaderStage)d->stage;
    WGPUSType language = (WGPUSType)d->language;

    WGPUShaderModule shaderModule = nullptr;
    if (language == WGPUSType_ShaderSourceGLSL) {
        std::vector<uint32_t> spirvSource;
        compileGLSL(source, d->stage, spirvSource);
        
        WGPUShaderSourceSPIRV shaderSourceSpirv = {
            .chain = {
                .sType = WGPUSType_ShaderSourceSPIRV
            },
            //TODO BUG: In uin32_t's, NOT in bytes
            .codeSize = (uint32_t)spirvSource.size() * 4,
            .code = spirvSource.data(),
        };

        WGPUShaderModuleDescriptor vertexDesc = { .nextInChain = &shaderSourceSpirv.chain };
        shaderModule = wgpuDeviceCreateShaderModule(device->device, &vertexDesc);
    }
    else {
        WGPUShaderSourceGLSL vertexCodeDesc = {
            .chain = {.sType = (WGPUSType)d->language },
            .stage = (WGPUShaderStage)stage,
            .code = {
                .data = source,
                .length = WGPU_STRLEN
            }
        };
        WGPUShaderModuleDescriptor vertexDesc = { .nextInChain = &vertexCodeDesc.chain };
        shaderModule = wgpuDeviceCreateShaderModule(device->device, &vertexDesc);
    }

    if (!shaderModule) {
        return nullptr;
    }

    WGShader* shader = new WGShader();
    shader->stage = d->stage;
    shader->shaderModule = shaderModule;

    WGPUReflectionInfoCallbackInfo reflectionCallbackInfo = {
            .nextInChain = NULL,
            .mode = WGPUCallbackMode_WaitAnyOnly,
            .callback = reflectionCallback,
            .userdata1 = shader,
            .userdata2 = NULL
    };
    WGPUFuture future = wgpuShaderModuleGetReflectionInfo(shaderModule, reflectionCallbackInfo);
    WGPUFutureWaitInfo futureWaitInfo = {
        .future = future,
        .completed = 0
    };
    wgpuInstanceWaitAny(device->instance, 1, &futureWaitInfo, 1000000000);

    return shader;
}

WGShader::~WGShader() {
    wgpuShaderModuleRelease(shaderModule);
}