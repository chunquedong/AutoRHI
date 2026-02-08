#ifndef WGTexture_H_
#define WGTexture_H_

#include "WGDevice.h"

namespace mrhi {

class WGSurface;

class WGTexture : public Texture {
    WGDevice* device = nullptr;
    TextureDesc desc;
public:
    WGPUTexture texture = nullptr;
    WGPUTextureView textureView = nullptr;
    WGSurface* fromSurface = nullptr;

    ~WGTexture();
    void init(WGDevice* device, const TextureDesc* desc);
    void setData(const void* data, int mipLevel, int depthOrArrayLayers) override;
};

class WGSampler : public Sampler {
    WGDevice* device = nullptr;
public:
    WGPUSampler sampler;

    ~WGSampler();
    void init(WGDevice* device, const SamplerDesc* desc);
};

}
#endif