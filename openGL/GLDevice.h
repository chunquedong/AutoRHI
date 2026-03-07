/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef GLDevice_H_
#define GLDevice_H_

#include "../rhi/GraphicsDevice.h"
#include "ogl.h"
#include "assert.h"

#ifndef STRVIEW
#define STRVIEW(X) GLPUStringView{X, sizeof(X) - 1}
#endif

namespace arhi {

class GLDevice : public GraphicsDevice {
public:

    void init();
    ~GLDevice();

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