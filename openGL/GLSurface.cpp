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

    auto tex = makeAPtr<GLTexture>();
    tex->fromSurface = this;
    textureView = std::move(tex);
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
APtr<Texture> GLSurface::getCurTextureView() {
    return arhi::share(textureView);
}
CommandEncoder* GLSurface::getCurCommandEncoder() {
    return commandEncoder.get();
}
APtr<FrameBuffer> GLSurface::getCurFrameBuffer() {
    return arhi::share(frameBuffer);
}
void GLSurface::cacheFrameBuffer(APtr<FrameBuffer> fbo) {
    frameBuffer = std::move(fbo);
}