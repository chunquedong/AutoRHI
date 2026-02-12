#include "GLSurface.h"
#include "GLTexture.h"
#include "GLCommandEncoder.h"

using namespace arhi;

GLSurface::~GLSurface() {
}

void GLSurface::init(GLDevice* device, const SurfaceDesc* desc) {
    GLint fbo = 0;
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo));

    this->frameBufferId = fbo;
    this->device = device;
    this->desc = *desc;

    commandEncoder = device->createCommandEncoder(CommandEncoderDesc{});

    GLTexture* tex = new GLTexture();
    tex->fromSurface = this;
    textureView = tex;
}

void GLSurface::resize(int w, int h) {
    this->desc.width = w;
    this->desc.height = h;
}

bool GLSurface::nextImage() {
    return true;
}

void GLSurface::present() {
}
Texture* GLSurface::getCurTextureView() {
    return textureView;
}
CommandEncoder* GLSurface::getCurCommandEncoder() {
    return commandEncoder;
}
FrameBuffer* GLSurface::getCurFrameBuffer() {
    return frameBuffer;
}
void GLSurface::cacheFrameBuffer(FrameBuffer* fbo) {
    if (frameBuffer) delete frameBuffer;
    frameBuffer = fbo;
}