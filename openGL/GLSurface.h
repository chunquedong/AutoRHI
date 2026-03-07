/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef GLSurface_H_
#define GLSurface_H_

#include "GLDevice.h"

namespace arhi {

class GLSurface : public Surface {
    GLDevice *device = nullptr;
    SurfaceDesc desc;

    APtr<Texture> textureView = nullptr;
    APtr<FrameBuffer> frameBuffer = nullptr;
    APtr<CommandEncoder> commandEncoder = nullptr;
public:
    GLuint frameBufferId = 0;

    ~GLSurface();
    void init(GLDevice* device, const SurfaceDesc* desc);
    
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