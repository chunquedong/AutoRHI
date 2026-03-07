#include "WGShader.h"

#include "spirv/rhi_spirv.h"

using namespace arhi;

#if 0
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
            ARHI_ERROR("Unsupport reflect uniform type\n");
            continue;
        }

#if 0
        char namebuffer[256] = { 0 };

        const WGPUGlobalReflectionInfo* toBePrinted = reflectionInfo->globals + i;

        memcpy(namebuffer, toBePrinted->name.data, toBePrinted->name.length);
        ARHI_ERROR("Name: %s, location: %u, type: %s", namebuffer, toBePrinted->binding, typedesc);
        if (reflectionInfo->globals[i].buffer.type != WGPUBufferBindingType_BindingNotUsed) {
            ARHI_ERROR(", minBindingSize = %d", (int)toBePrinted->buffer.minBindingSize);
        }
        ARHI_ERROR("\n");
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
            //ARHI_ERROR("input attribute: %d, %d\n", info->location, attri.format);
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
            //ARHI_ERROR("output attribute: %d, %d\n", info->location, attri.format);
            shader->reflection.outputAttrs.push_back(attri);
        }
    }
}

#endif

APtr<WGShader> WGShader::create(WGDevice* device, const ShaderDesc* d) {

    const char* source = d->source;
    WGPUShaderStage stage = (WGPUShaderStage)d->stage;

    std::vector<uint32_t> spirvSource;
    WGPUShaderModule shaderModule = nullptr;

    if (d->language == ShaderLanguage::GLSL) {
        compileGLSL(source, d->stage, spirvSource);
    }
    else if (d->language == ShaderLanguage::SPIRV) {
        // For SPIR-V input, create the shader module directly from the SPIR-V data
        const uint32_t* spirvCode = reinterpret_cast<const uint32_t*>(source);
        size_t spirvSize = d->sourceLen / sizeof(uint32_t);
        
        spirvSource.resize(spirvSize);
        memcpy(spirvSource.data(), spirvCode, d->sourceLen);
    }
    else if (d->language == ShaderLanguage::WGSL) {
        compileWGSL(source, d->stage, spirvSource);
    }
    else {
        ARHI_ERROR("Unsupport Shader Language %d\n", d->language);
        return nullptr;
    }

    std::string wgslCode;
    if (spirvSource.size() > 0) {
#ifdef __EMSCRIPTEN__
        // if (!spirvToWGSL((const char*)spirvSource.data(), spirvSource.size()*sizeof(uint32_t), wgslCode)) {
        //     ARHI_ERROR("spirvToWgsl failed\n");
        //     return nullptr;
        // }
        // source = wgslCode.c_str();
        // printf("WGSL: %s\n", source);
#else
        WGPUShaderSourceSPIRV shaderSourceSpirv = {
            .chain = {
                .sType = WGPUSType_ShaderSourceSPIRV
            },
            .codeSize = (uint32_t)spirvSource.size(),
            .code = spirvSource.data(),
        };

        WGPUShaderModuleDescriptor vertexDesc = { .nextInChain = &shaderSourceSpirv.chain };
        shaderModule = wgpuDeviceCreateShaderModule(device->device, &vertexDesc);
#endif
    }

    if (!shaderModule && (d->language == ShaderLanguage::WGSL || wgslCode.size() > 0)) {
        WGPUShaderSourceWGSL vertexCodeDesc = {
            .chain = {.sType = WGPUSType_ShaderSourceWGSL },
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

    auto shader = makeAPtr<WGShader>();
    shader->stage = d->stage;
    shader->shaderModule = shaderModule;
#if 0
    WGPUReflectionInfoCallbackInfo reflectionCallbackInfo = {
            .nextInChain = NULL,
            .mode = WGPUCallbackMode_WaitAnyOnly,
            .callback = reflectionCallback,
            .userdata1 = shader.get(),
            .userdata2 = NULL
    };
    WGPUFuture future = wgpuShaderModuleGetReflectionInfo(shaderModule, reflectionCallbackInfo);
    WGPUFutureWaitInfo futureWaitInfo = {
        .future = future,
        .completed = 0
    };
    wgpuInstanceWaitAny(device->instance, 1, &futureWaitInfo, 1000000000);
#else
    if (spirvSource.size() > 0) {
        spirvReflect(spirvSource, d->stage, shader->reflection);
    }
#endif
    return shader;
}

WGShader::~WGShader() {
    wgpuShaderModuleRelease(shaderModule);
}