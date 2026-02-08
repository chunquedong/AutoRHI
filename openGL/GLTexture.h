#ifndef GLTexture_H_
#define GLTexture_H_

#include "GLDevice.h"

namespace mrhi {

class GLSurface;
class GLSampler;

class GLTexture : public Texture {
    GLDevice* device = nullptr;

public:
    TextureDesc desc;
    GLuint texture = 0;
    GLenum targetType = 0;
    GLSurface* fromSurface = nullptr;
    GLSampler* combinedSampler = nullptr;

    ~GLTexture();
    void init(GLDevice* device, const TextureDesc* desc);
    void setData(const void* data, int mipLevel, int depthOrArrayLayers) override;
};

class GLSampler : public Sampler {
    GLDevice* device = nullptr;
public:
    GLuint sampler = 0;

    ~GLSampler();
    void init(GLDevice* device, const SamplerDesc* desc);
};

}
#endif