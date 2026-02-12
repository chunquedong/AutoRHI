#include "WGTexture.h"

using namespace arhi;

void WGTexture::init(WGDevice* adevice, const TextureDesc* desc) {
    WGPUDevice device = adevice->device;
    this->desc = *desc;

    WGPUTextureDescriptor tdesc = {
            .usage = (unsigned long long)desc->usage,
            .dimension = (WGPUTextureDimension)desc->dimension,
            .size = {(unsigned int)desc->width, (unsigned int)desc->height, (unsigned int)desc->depthOrArrayLayers},
            .format = (WGPUTextureFormat)desc->format,
            .mipLevelCount = (unsigned int)desc->mipLevelCount,
            .sampleCount = 1
    };
    WGPUTexture texture = wgpuDeviceCreateTexture(device, &tdesc);


    WGPUTextureViewDescriptor viewDesc = {
        .format = (WGPUTextureFormat)desc->format,
        .dimension = (WGPUTextureViewDimension)desc->type,
        .baseMipLevel = (unsigned int)desc->baseMipLevel,
        .mipLevelCount = (unsigned int)desc->mipLevelCount,
        .baseArrayLayer = (unsigned int)desc->baseArrayLayer,
        .arrayLayerCount = (unsigned int)desc->arrayLayerCount,
        .aspect = WGPUTextureAspect_All,
        .usage = WGPUTextureUsage_TextureBinding
    };
    WGPUTextureView textureView = wgpuTextureCreateView(texture, &viewDesc);

    this->device = adevice;
    this->texture = texture;
    this->textureView = textureView;
}

void WGTexture::setData(const void* textureData, int mipLevel, int depthOrArrayLayers) {
    if (!texture) {
        MGP_ERROR("Null Texture\n");
        abort();
        return;
    }

    WGPUTexelCopyTextureInfo copyTextureInfo = {
        .texture = texture,
        .mipLevel = (uint32_t)mipLevel,
        .origin = {.z = (uint32_t)depthOrArrayLayers },
        .aspect = WGPUTextureAspect_All,
    };
    WGPUTexelCopyBufferLayout copyBufferLayout = {
        .bytesPerRow = (unsigned int)desc.width * desc.bytePerPixel, .rowsPerImage = (unsigned int)desc.height
    };
    WGPUExtent3D extent3d = {
        desc.width, desc.height, 1
    };
    wgpuQueueWriteTexture(
        device->queue,
        &copyTextureInfo,
        textureData, (unsigned int)desc.width * desc.height * desc.bytePerPixel,
        &copyBufferLayout,
        &extent3d
    );
    //free(textureData);
}

WGTexture::~WGTexture() {
    if (texture) wgpuTextureRelease(texture);
    wgpuTextureViewRelease(textureView);
}

WGSampler::~WGSampler()
{
    wgpuSamplerRelease(sampler);
}

void WGSampler::init(WGDevice* device, const SamplerDesc* desc)
{
    WGPUSamplerDescriptor samplerDesc = {
        .addressModeU = (WGPUAddressMode)desc->addressModeU,
        .addressModeV = (WGPUAddressMode)desc->addressModeV,
        .addressModeW = (WGPUAddressMode)desc->addressModeW,
        .magFilter = (WGPUFilterMode)desc->magFilter,
        .minFilter = (WGPUFilterMode)desc->minFilter,
        .mipmapFilter = (WGPUMipmapFilterMode)desc->mipmapFilter,
        .lodMinClamp = desc->lodMinClamp,
        .lodMaxClamp = desc->lodMinClamp,
        .compare = (WGPUCompareFunction)desc->compare,
        .maxAnisotropy = desc->maxAnisotropy,
    };
    WGPUSampler sampler = wgpuDeviceCreateSampler(device->device, &samplerDesc);
    this->sampler = sampler;
}

