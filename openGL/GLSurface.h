#ifndef GLSurface_H_
#define GLSurface_H_

#include "GLDevice.h"

namespace arhi {

class GLSurface : public Surface {
    GLDevice *device = nullptr;
    SurfaceDesc desc;

    Texture* textureView = nullptr;
    FrameBuffer* frameBuffer = nullptr;
    CommandEncoder* commandEncoder = nullptr;
public:
    GLuint frameBufferId = 0;

    ~GLSurface();
    void init(GLDevice* device, const SurfaceDesc* desc);
    
    void resize(int w, int h) override;

    bool nextImage() override;

    void present() override;

    Texture* getCurTextureView() override;
    CommandEncoder* getCurCommandEncoder() override;
    FrameBuffer* getCurFrameBuffer() override;
    void cacheFrameBuffer(FrameBuffer* fbo) override;

};

}
#endif