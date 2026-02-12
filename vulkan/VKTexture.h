#ifndef VKTexture_H_
#define VKTexture_H_

#include "VKDevice.h"

namespace arhi {

class FrameState;

class VKTexture : public Texture {
public:
    VKDevice* device = nullptr;
public:
    TextureDesc desc;

    VkImage textureImage = nullptr;
    VkDeviceMemory textureImageMemory = nullptr;
    VkImageView textureView = nullptr;
    //VkSampler textureSampler;

public:
    FrameState* frameState = nullptr;

    ~VKTexture();
    void init(VKDevice* device, const TextureDesc* desc);
    void setData(const void* data, int mipLevel, int depthOrArrayLayers) override;
private:
};

class VKSampler : public Sampler {
    VKDevice* device = nullptr;
public:
    VkSampler sampler;

    ~VKSampler();
    void init(VKDevice* device, const SamplerDesc* desc);
};

}
#endif