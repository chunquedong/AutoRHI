#ifndef GraphicsContext_H_
#define GraphicsContext_H_

#include "Resource.h"
#include "Shader.h"
#include "Pipeline.h"

namespace mrhi {

struct BindingEntry {
    std::string name;
    Resource* resource;
    //int binding = -1;
};

struct BindingGroupDesc {
    int bindingGroup = 0;
    Pipeline* pipeline = nullptr;
    std::vector<BindingEntry> resources;
};

struct BindingGroup {
    virtual ~BindingGroup() {}
};

enum struct LoadOp {
    Undefined = 0x00000000,
    Load = 0x00000001,
    Clear = 0x00000002,
    ExpandResolveTexture = 0x00050003,
};

enum struct StoreOp {
    Undefined = 0x00000000,
    Store = 0x00000001,
    Discard = 0x00000002,
};

struct RenderPassColorAttachment {
    Texture* view;
    //uint32_t depthSlice;
    LoadOp loadOp;
    StoreOp storeOp;
    //rgba
    float clearValue[4];
};

struct RenderPassDepthStencilAttachment {
    Texture* view;
    LoadOp depthLoadOp;
    StoreOp depthStoreOp;
    float depthClearValue;
    uint32_t depthReadOnly;
    LoadOp stencilLoadOp;
    StoreOp stencilStoreOp;
    uint32_t stencilClearValue;
    uint32_t stencilReadOnly;
};

struct RenderPassDesc {
    const char* label = nullptr;
    std::vector<RenderPassColorAttachment> colorAttachments;
    const RenderPassDepthStencilAttachment* depthStencilAttachment = nullptr;
};

struct FrameBuffer {
    virtual ~FrameBuffer() {}
};

struct CommandEncoderDesc {

};


struct CommandEncoder {
    virtual ~CommandEncoder() {}
    //virtual bool beginPass(const RenderPassDesc& desc) = 0;
    virtual bool beginPass(FrameBuffer* frameBuffer) = 0;
    virtual void endPass() = 0;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) = 0;
    virtual void setIndexBuffer(Buffer*, int offset, IndexFormat indexFormat) = 0;
    virtual void setVertexBuffer(Buffer*, int offset, int binding) = 0;
    virtual void drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) = 0;

    virtual void setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;

    virtual void submit() = 0;
};

struct GraphicsDevice {
    static GraphicsDevice* cur();
    virtual ~GraphicsDevice() {}

    virtual Surface* createSurface(const SurfaceDesc& desc) = 0;
    virtual Pipeline* createPipeline(const PipelineDesc& desc) = 0;
    virtual Texture* createTexture(const TextureDesc& desc) = 0;
    virtual Buffer* createBuffer(const BufferDesc& desc) = 0;
    virtual CommandEncoder* createCommandEncoder(const CommandEncoderDesc& desc) = 0;
    virtual Shader* createShader(const ShaderDesc& desc) = 0;
    virtual Sampler* createSampler(const SamplerDesc& desc) = 0;
    virtual BindingGroup* createBindingGroup(const BindingGroupDesc& desc) = 0;
    virtual FrameBuffer* createFrameBuffer(const RenderPassDesc& desc) = 0;
    virtual void waitIdle() {}
};

}
#endif