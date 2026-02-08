#ifndef WGDevice_H_
#define WGDevice_H_

#include "rhi/GraphicsDevice.h"
#include <webgpu/webgpu.h>

namespace mrhi {

class WGDevice : public GraphicsDevice {
public:
    WGPUInstance instance;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    //WGPUChainedStruct* surfaceChain = nullptr;

    void init();
    ~WGDevice();

    Surface* createSurface(const SurfaceDesc& desc) override;
    Pipeline* createPipeline(const PipelineDesc& desc) override;
    Texture* createTexture(const TextureDesc& desc) override;
    Buffer* createBuffer(const BufferDesc& desc) override;
    CommandEncoder* createCommandEncoder(const CommandEncoderDesc& desc) override;
    Shader* createShader(const ShaderDesc& desc) override;
    Sampler* createSampler(const SamplerDesc& desc) override;
    BindingGroup* createBindingGroup(const BindingGroupDesc& desc) override;
    FrameBuffer* createFrameBuffer(const RenderPassDesc& desc) override;
};

}
#endif