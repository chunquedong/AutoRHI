#include "GLDevice.h"
#include "GLCommandEncoder.h"

using namespace mrhi;

extern GraphicsDevice* g_graphicsDevice;

void GLDevice::init() {
    g_graphicsDevice = this;
}

GLDevice::~GLDevice() {
    if (g_graphicsDevice == this) {
        g_graphicsDevice = nullptr;
    }
}


Surface* GLDevice::createSurface(const SurfaceDesc& d) {
    GLSurface* fbo = new GLSurface();
    fbo->init(this, &d);
    return fbo;
}

Pipeline* GLDevice::createPipeline(const PipelineDesc& d) {
    return GLPipeline::create(this, &d);
}

Texture* GLDevice::createTexture(const TextureDesc& d) {
    GLTexture* t = new GLTexture();
    t->init(this, &d);
    return t;
}

Buffer* GLDevice::createBuffer(const BufferDesc& d) {
    GLBuffer* t = new GLBuffer();
    t->init(this, &d);
    return t;
}

CommandEncoder* GLDevice::createCommandEncoder(const CommandEncoderDesc& d) {
    GLCommandEncoder* t = new GLCommandEncoder();
    t->init(this, &d);
    return t;
}

Shader* GLDevice::createShader(const ShaderDesc& d) {
    return GLShader::create(this, &d);
}

Sampler* GLDevice::createSampler(const SamplerDesc& d) {
    GLSampler* t = new GLSampler();
    t->init(this, &d);
    return t;
}

BindingGroup* GLDevice::createBindingGroup(const BindingGroupDesc& d) {
    GLBindingGroup* t = new GLBindingGroup();
    t->init(this, &d);
    return t;
}

FrameBuffer* GLDevice::createFrameBuffer(const RenderPassDesc& desc)
{
    GLFrameBuffer* t = new GLFrameBuffer();
    t->update(this, desc);
    return t;
}
