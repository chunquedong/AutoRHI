/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef WGDevice_H_
#define WGDevice_H_

#include "rhi/GraphicsDevice.h"
#include <webgpu/webgpu.h>

namespace arhi {

class WGDevice : public GraphicsDevice {
public:
    WGPUInstance instance;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    //WGPUChainedStruct* surfaceChain = nullptr;

    void init();
    ~WGDevice();

    APtr<Surface> createSurface(const SurfaceDesc& desc) override;
    APtr<Pipeline> doCreatePipeline(const PipelineDesc& desc) override;
    APtr<Texture> createTexture(const TextureDesc& desc) override;
    APtr<Buffer> createBuffer(const BufferDesc& desc) override;
    APtr<CommandEncoder> createCommandEncoder(const CommandEncoderDesc& desc) override;
    APtr<Shader> createShader(const ShaderDesc& desc) override;
    APtr<Sampler> createSampler(const SamplerDesc& desc) override;
    APtr<BindingGroup> createBindingGroup(BindingGroupDesc&& desc) override;
    APtr<FrameBuffer> createFrameBuffer(RenderPassDesc&& desc) override;


};

}
#endif