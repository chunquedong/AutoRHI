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