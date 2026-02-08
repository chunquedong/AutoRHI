#ifndef WGSurface_H_
#define WGSurface_H_

#include "WGDevice.h"

namespace mrhi {

class WGTexture;

class WGSurface : public Surface {
    WGDevice *device = nullptr;
    SurfaceDesc desc;

    WGTexture* textureView = nullptr;
    FrameBuffer* frameBuffer = nullptr;
    CommandEncoder* commandEncoder = nullptr;
public:
    WGPUSurface surface = nullptr;
    //WGPUTextureView surfaceView = nullptr;

    ~WGSurface();
    void init(WGDevice* device, const SurfaceDesc* desc);
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