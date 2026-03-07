/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef WGSurface_H_
#define WGSurface_H_

#include "WGDevice.h"

namespace arhi {

class WGTexture;

class WGSurface : public Surface {
    WGDevice *device = nullptr;
    SurfaceDesc desc;

    APtr<WGTexture> textureView = nullptr;
    APtr<FrameBuffer> frameBuffer = nullptr;
    APtr<CommandEncoder> commandEncoder = nullptr;
public:
    WGPUSurface surface = nullptr;
    //WGPUTextureView surfaceView = nullptr;

    ~WGSurface();
    void init(WGDevice* device, const SurfaceDesc* desc);
    void resize(int w, int h) override;

    bool nextImage() override;

    void present() override;

    APtr<Texture> getCurTextureView() override;
    CommandEncoder* getCurCommandEncoder() override;
    APtr<FrameBuffer> getCurFrameBuffer() override;
    void cacheFrameBuffer(APtr<FrameBuffer> fbo) override;
};

}
#endif