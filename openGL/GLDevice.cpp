#include "GLDevice.h"
#include "GLCommandEncoder.h"

using namespace arhi;

extern GraphicsDevice* g_graphicsDevice;

void GLDevice::init() {
    g_graphicsDevice = this;
}

GLDevice::~GLDevice() {
    if (g_graphicsDevice == this) {
        g_graphicsDevice = nullptr;
    }
}


APtr<Surface> GLDevice::createSurface(const SurfaceDesc& d) {
    auto fbo = makeAPtr<GLSurface>();
    fbo->init(this, &d);
    return fbo;
}

APtr<Pipeline> GLDevice::doCreatePipeline(const PipelineDesc& d) {
    auto pipeline = GLPipeline::create(this, &d);
    return pipeline;
}

APtr<Texture> GLDevice::createTexture(const TextureDesc& d) {
    auto t = makeAPtr<GLTexture>();
    t->init(this, &d);
    return t;
}

APtr<Buffer> GLDevice::createBuffer(const BufferDesc& d) {
    auto t = makeAPtr<GLBuffer>();
    t->init(this, &d);
    return t;
}

APtr<CommandEncoder> GLDevice::createCommandEncoder(const CommandEncoderDesc& d) {
    auto t = makeAPtr<GLCommandEncoder>();
    t->init(this, &d);
    return t;
}

APtr<Shader> GLDevice::createShader(const ShaderDesc& d) {
    auto shader = GLShader::create(this, &d);
    return shader;
}

APtr<Sampler> GLDevice::createSampler(const SamplerDesc& d) {
    auto t = makeAPtr<GLSampler>();
    t->init(this, &d);
    return t;
}

APtr<BindingGroup> GLDevice::createBindingGroup(BindingGroupDesc&& d) {
    auto t = makeAPtr<GLBindingGroup>();
    t->init(this, std::move(d));
    return t;
}

APtr<FrameBuffer> GLDevice::createFrameBuffer(RenderPassDesc&& desc)
{
    auto t = makeAPtr<GLFrameBuffer>();
    t->update(this, std::move(desc));
    return t;
}
