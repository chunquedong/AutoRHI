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
                .type = WGPUBufferBindingType_Uniform,
                .hasDynamicOffset = false,
                .minBindingSize = (unsigned long long)uniform.bufferMinBindingSize,
            };
            if (uniform.bufferBindingType == BufferDesc::Usage_Storage) {
                layoutEntry.buffer.type = WGPUBufferBindingType_Storage;
            }
        }
        else {
            ARHI_ERROR("ERROR: unknow uniform type %d\n", uniform.type);
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
                    ARHI_ERROR("Not found attribute %d\n", bufferInfo._location);
                    continue;
                }
                attr.format = (WGPUVertexFormat)shaderFound->second.format;
                attr.shaderLocation = bufferInfo._location;
            }
            else {
                auto shaderFound = this->reflection.attributesIndex.find(bufferInfo.name);
                if (shaderFound == this->reflection.attributesIndex.end()) {
                    ARHI_ERROR("Not found attribute %s\n", bufferInfo.name.c_str());
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
        WGPUColorTargetState wgState = {};
        wgState.format = (WGPUTextureFormat)state.format;
        wgState.writeMask = (WGPUColorWriteMask)state.writeMask;
        if (state.blend) {
            blendState = *((WGPUBlendState*)state.blend);
            wgState.blend = &blendState;
        }
        colorTargetState.push_back(wgState);
    }

    WGPUFragmentState fragmentState = {};
    if (fragmentShader) {
        fragmentState.module = fragmentShader->shaderModule;
        fragmentState.entryPoint.data = fragmentShader->entryPoint;
        fragmentState.entryPoint.length = fragmentShader->entryPoint?strlen(fragmentShader->entryPoint):0;
        fragmentState.targetCount = colorTargetState.size();
        fragmentState.targets = colorTargetState.data();
    }

    // Set up depth stencil state if provided
    WGPUDepthStencilState depthStencilState = {};
    WGPUDepthStencilState* pDepthStencilState = nullptr;
    
    if (desc->depthStencil) {
        const DepthStencilState& ds = *desc->depthStencil;
        depthStencilState.format = (WGPUTextureFormat)ds.format;
        depthStencilState.depthWriteEnabled = ds.depthWriteEnabled ? WGPUOptionalBool_True : WGPUOptionalBool_False;
        depthStencilState.depthCompare = (WGPUCompareFunction)ds.depthCompare;
        depthStencilState.depthBias = ds.depthBias;
        depthStencilState.depthBiasSlopeScale = ds.depthBiasSlopeScale;
        depthStencilState.depthBiasClamp = ds.depthBiasClamp;
        depthStencilState.stencilReadMask = ds.stencilReadMask;
        depthStencilState.stencilWriteMask = ds.stencilWriteMask;
        
        depthStencilState.stencilFront.compare = (WGPUCompareFunction)ds.stencilFront.compare;
        depthStencilState.stencilFront.failOp = (WGPUStencilOperation)ds.stencilFront.failOp;
        depthStencilState.stencilFront.depthFailOp = (WGPUStencilOperation)ds.stencilFront.depthFailOp;
        depthStencilState.stencilFront.passOp = (WGPUStencilOperation)ds.stencilFront.passOp;
        
        depthStencilState.stencilBack.compare = (WGPUCompareFunction)ds.stencilBack.compare;
        depthStencilState.stencilBack.failOp = (WGPUStencilOperation)ds.stencilBack.failOp;
        depthStencilState.stencilBack.depthFailOp = (WGPUStencilOperation)ds.stencilBack.depthFailOp;
        depthStencilState.stencilBack.passOp = (WGPUStencilOperation)ds.stencilBack.passOp;
        
        pDepthStencilState = &depthStencilState;
    }

    // Render Pipeline
    WGPURenderPipelineDescriptor rpdesc = {};
    rpdesc.label.data = desc->label;
    rpdesc.label.length = desc->label?strlen(desc->label):0;
    rpdesc.layout = pllayout;
    
    rpdesc.vertex.module = vertexShader->shaderModule;
    rpdesc.vertex.entryPoint.data = vertexShader->entryPoint;
    rpdesc.vertex.entryPoint.length = vertexShader->entryPoint?strlen(vertexShader->entryPoint):0;
    rpdesc.vertex.bufferCount = vertexBufferLayoutList.size();
    rpdesc.vertex.buffers = vertexBufferLayoutList.data();
    
    rpdesc.primitive.topology = (WGPUPrimitiveTopology)desc->primitive.topology;
    rpdesc.primitive.frontFace = (WGPUFrontFace)desc->primitive.frontFace;
    rpdesc.primitive.cullMode = (WGPUCullMode)desc->primitive.cullMode;
    //rpdesc.primitive.polygonMode = (WGPUPolygonMode)desc->primitive.polygonMode;
    rpdesc.primitive.unclippedDepth = desc->primitive.unclippedDepth;
    
    rpdesc.depthStencil = pDepthStencilState;
    
    rpdesc.multisample.count = desc->multisample.count;
    rpdesc.multisample.mask = desc->multisample.mask;
    rpdesc.multisample.alphaToCoverageEnabled = desc->multisample.alphaToCoverageEnabled;
    
    rpdesc.fragment = fragmentShader ? &fragmentState : nullptr;
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