#include "WGPipeline.h"
#include "WGShader.h"

#include <map>

using namespace arhi;


void WGPipeline::buildBindGroupLayoutList(WGDevice* adevice) {
    std::map<int, std::vector<WGPUBindGroupLayoutEntry>> group;
    for (auto it = reflection.uniforms.begin(); it != reflection.uniforms.end(); ++it) {
        const UniformVar& uniform = it->second;
        std::vector<WGPUBindGroupLayoutEntry>& layoutEntries = group[uniform.bindGroup];

        WGPUBindGroupLayoutEntry layoutEntry = {};
        layoutEntry.binding = uniform.binding;
        layoutEntry.visibility = uniform.visibility;
        if (uniform.type == UniformType::Texture) {
            layoutEntry.texture = WGPUTextureBindingLayout{
                .sampleType = (WGPUTextureSampleType)uniform.textureSampleType ,
                .viewDimension = (WGPUTextureViewDimension)uniform.textureViewDimension,
            };
        }
        else if (uniform.type == UniformType::Sampler) {
            layoutEntry.sampler = WGPUSamplerBindingLayout{
                .type = (WGPUSamplerBindingType)uniform.samplerBindingType
            };
        }
        else if (uniform.type == UniformType::UniformBuffer) {
            layoutEntry.buffer = WGPUBufferBindingLayout {
                .type = (WGPUBufferBindingType)uniform.bufferBindingType,
                .hasDynamicOffset = false,
                .minBindingSize = (unsigned long long)uniform.bufferMinBindingSize,
            };
        }
        else {
            MGP_ERROR("ERROR: unknow uniform type %d\n", uniform.type);
            continue;
        }

        layoutEntries.push_back(layoutEntry);
    }

    bindGroupLayoutList.clear();
    for (auto it = group.begin(); it != group.end(); ++it) {
        std::vector<WGPUBindGroupLayoutEntry>& layoutEntries = it->second;

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = { .entryCount = layoutEntries.size(), .entries = layoutEntries.data(), };
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(adevice->device, &bindGroupLayoutDesc);
        bindGroupLayoutList.push_back(bindGroupLayout);
    }
}

void WGPipeline::createVertexInputLayout(const PipelineDesc* desc, 
    std::vector<WGPUVertexBufferLayout>& vertexBufferLayoutList,
    std::vector<WGPUVertexAttribute>& vertexAttributes) {
    for (auto& vertexFormat : desc->bufferLayout) {
        int arrayStride = vertexFormat.stride;
        int begin = vertexAttributes.size();
        for (int i = 0; i < vertexFormat.elements.size(); ++i) {
            const AttributeElement& bufferInfo = vertexFormat.elements[i];
            WGPUVertexAttribute attr = {};
            if (bufferInfo._location != -1) {
                auto shaderFound = this->reflection.attributes.find(bufferInfo._location);
                if (shaderFound == this->reflection.attributes.end()) {
                    MGP_ERROR("Not found attribute %d\n", bufferInfo._location);
                    continue;
                }
                attr.format = (WGPUVertexFormat)shaderFound->second.format;
                attr.shaderLocation = bufferInfo._location;
            }
            else {
                auto shaderFound = this->reflection.attributesIndex.find(bufferInfo.name);
                if (shaderFound == this->reflection.attributesIndex.end()) {
                    MGP_ERROR("Not found attribute %s\n", bufferInfo.name.c_str());
                    continue;
                }
                attr.format = (WGPUVertexFormat)shaderFound->second.format;
                attr.shaderLocation = shaderFound->second.location;
            }

            attr.offset = bufferInfo.offset;
            //arrayStride = bufferInfo.stride;
            vertexAttributes.push_back(attr);
        }

        WGPUVertexBufferLayout vbLayout = {
            .stepMode = (WGPUVertexStepMode)vertexFormat.stepMode,
            .arrayStride = (uint64_t)arrayStride,
            .attributeCount = vertexAttributes.size() - begin,
            .attributes = vertexAttributes.data() + begin,
        };
        vertexBufferLayoutList.push_back(vbLayout);
    }
}

bool WGPipeline::init(WGDevice* adevice, const PipelineDesc* desc) {
    WGPUDevice device = adevice->device;
    WGPUQueue queue = adevice->queue;

    WGShader* vertexShader = dynamic_cast<WGShader*>(desc->vertexShader);
    WGShader* fragmentShader = nullptr;
    if (desc->fragmentShader) {
        fragmentShader = dynamic_cast<WGShader*>(desc->fragmentShader);
    }

    //reflect
    reflect(desc);

    // Vertex Layout
    std::vector<WGPUVertexBufferLayout> vertexBufferLayoutList;
    std::vector<WGPUVertexAttribute> vertexAttributes;
    createVertexInputLayout(desc, vertexBufferLayoutList, vertexAttributes);


    buildBindGroupLayoutList(adevice);
    WGPUPipelineLayoutDescriptor pldesc = {
        .bindGroupLayoutCount = bindGroupLayoutList.size(),
        .bindGroupLayouts = bindGroupLayoutList.data()
    };
    WGPUPipelineLayout pllayout = wgpuDeviceCreatePipelineLayout(device, &pldesc);

    std::vector<WGPUColorTargetState> colorTargetState;
    WGPUBlendState blendState = {};
    for (const ColorTargetState& state : desc->targets) {
        WGPUColorTargetState wgState = {
            .format = (WGPUTextureFormat)state.format,
            .writeMask = (WGPUColorWriteMask)state.writeMask,
        };
        if (state.blend) {
            blendState = *((WGPUBlendState*)state.blend);
            wgState.blend = &blendState;
        }
        colorTargetState.push_back(wgState);
    }

    WGPUFragmentState fragmentState;
    if (fragmentShader) {
        fragmentState = {
            .module = fragmentShader->shaderModule,
            .entryPoint = {
                .data = fragmentShader->entryPoint,
                .length = fragmentShader->entryPoint?strlen(fragmentShader->entryPoint):0
            },
            /*.constantCount = 1,
            .constants = &brightnessConstant,*/
            .targetCount = colorTargetState.size(),
            .targets = colorTargetState.data(),
        };
    }

    // Render Pipeline
    WGPURenderPipelineDescriptor rpdesc = {
        .label = {.data = desc->label, .length = desc->label?strlen(desc->label):0 },
        .layout = pllayout,
        .vertex = {
            .module = vertexShader->shaderModule,
            .entryPoint = {
                .data = vertexShader->entryPoint,
                .length = vertexShader->entryPoint?strlen(vertexShader->entryPoint):0
            },
            .bufferCount = vertexBufferLayoutList.size(),
            .buffers = vertexBufferLayoutList.data(),
        },
        .primitive = {
            .topology = (WGPUPrimitiveTopology)desc->primitive.topology,
            .frontFace = (WGPUFrontFace)desc->primitive.frontFace,
            .cullMode = (WGPUCullMode)desc->primitive.cullMode,
        },
        .multisample = {
            .count = desc->multisample.count,
            .mask = desc->multisample.mask,
        },
        .fragment = fragmentShader  ? &fragmentState : nullptr,
    };
    this->pipeline = wgpuDeviceCreateRenderPipeline(device, &rpdesc);

    wgpuPipelineLayoutRelease(pllayout);

    return true;
}

WGPipeline::~WGPipeline() {
    wgpuRenderPipelineRelease(pipeline);

    for (auto& it : bindGroupLayoutList) {
        wgpuBindGroupLayoutRelease(bindGroupLayoutList[0]);
    }
    bindGroupLayoutList.clear();
}