/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef WGCommandEncoder_H_
#define WGCommandEncoder_H_

#include "WGDevice.h"
#include "WGSurface.h"
#include "WGPipeline.h"
#include "WGTexture.h"
#include "WGBuffer.h"

namespace arhi {

class WGBindingGroup : public BindingGroup {
public:
    //BindingGroupDesc desc;
    WGPUBindGroup bindGroup;

    void init(WGDevice* device, BindingGroupDesc&& desc);
    virtual ~WGBindingGroup();
};

class WGFrameBuffer : public FrameBuffer {
public:
    RenderPassDesc desc;
};

class WGCommandEncoder : public CommandEncoder {
public:
    WGDevice *device;
    
    WGPUCommandEncoder commandEncoder;
    WGPURenderPassEncoder renderPassEncoder;

    WGPipeline* curPipeline = nullptr;
public:
    ~WGCommandEncoder();
    bool beginPass(FrameBuffer* frameBuffer) override;
    bool beginPass(const RenderPassDesc& desc);
    void endPass() override;
    void init(WGDevice* device, const CommandEncoderDesc* desc);
    void setPipeline(Pipeline* pipeline) override;
    void setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) override;
    void setIndexBuffer(Buffer* buffer, int offset, IndexFormat indexFormat) override;
    void setVertexBuffer(Buffer* buffer, int offset, int binding) override;
    void drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) override;

    void setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
    void submit() override;
private:

};

}

#endif