#include "WGSurface.h"
#include "WGTexture.h"

using namespace arhi;

WGSurface::~WGSurface() {
    //wgpuTextureViewRelease(surfaceView);
    wgpuSurfaceRelease(surface);
}

void WGSurface::init(WGDevice* device, const SurfaceDesc* desc) {
    WGPUSurfaceDescriptor surfaceDescriptor = {
        .nextInChain = (WGPUChainedStruct*)(desc->surfaceChain),
    };

    WGPUSurfaceCapabilities caps = { 0 };
#ifdef __EMSCRIPTEN__
    WGPUPresentMode desiredPresentMode = WGPUPresentMode_Fifo;
#else
    WGPUPresentMode desiredPresentMode = WGPUPresentMode_Immediate;
#endif
    
    WGPUSurface surface = wgpuInstanceCreateSurface(device->instance, &surfaceDescriptor);

    wgpuSurfaceGetCapabilities(surface, device->adapter, &caps);

    WGPUSurfaceConfiguration configureation = {
        .device = device->device,
        .format = (WGPUTextureFormat)desc->format,
        .usage = WGPUTextureUsage_RenderAttachment,
        .width = (uint32_t)desc->width,
        .height = (uint32_t)desc->height,
        .alphaMode = (WGPUCompositeAlphaMode)desc->alphaMode,
        .presentMode = desiredPresentMode,
    };
    wgpuSurfaceConfigure(surface, &configureation);

    this->surface = surface;
    this->device = device;
    this->desc = *desc;
}

void WGSurface::resize(int w, int h) {
    WGPUSurfaceCapabilities caps = { 0 };
    WGPUPresentMode desiredPresentMode = WGPUPresentMode_Immediate;//WGPUPresentMode_Fifo

    desc.width = w;
    desc.height = h;

    WGPUSurfaceConfiguration surfaceConfig = {
        .device = device->device,
        .format = (WGPUTextureFormat)desc.format,
        .usage = WGPUTextureUsage_RenderAttachment,
        .width = (uint32_t)w,
        .height = (uint32_t)h,
        .alphaMode = (WGPUCompositeAlphaMode)desc.alphaMode,
        .presentMode = desiredPresentMode,
    };

    wgpuSurfaceConfigure(surface, &surfaceConfig);
}

bool WGSurface::nextImage() {
    auto tex = makeAPtr<WGTexture>();

    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal) {
        return false;
    }
    WGPUTextureViewDescriptor descriptor = WGPUTextureViewDescriptor{
        .format = (WGPUTextureFormat)desc.format,
        .dimension = WGPUTextureViewDimension_2D,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
        .aspect = WGPUTextureAspect_All,
        .usage = WGPUTextureUsage_RenderAttachment,
    };

    tex->fromSurface = this;
    tex->texture = nullptr;
    tex->textureView = wgpuTextureCreateView(surfaceTexture.texture, &descriptor);

    this->textureView = std::move(tex);

    return true;
}

void WGSurface::present() {
#ifndef __EMSCRIPTEN__
    wgpuSurfacePresent(this->surface);
#endif

    // Smart pointers will automatically manage memory, no need for manual deletion
    textureView = nullptr;
    frameBuffer = nullptr;
    commandEncoder = nullptr;
}
APtr<Texture> WGSurface::getCurTextureView() {
    return arhi::share(this->textureView);
}
CommandEncoder* WGSurface::getCurCommandEncoder() {
    auto cmd = device->createCommandEncoder(CommandEncoderDesc{});
    commandEncoder = std::move(cmd);
    return commandEncoder.get();
}
APtr<FrameBuffer> WGSurface::getCurFrameBuffer() {
    return arhi::share(frameBuffer);
}
void WGSurface::cacheFrameBuffer(APtr<FrameBuffer> fbo) {
    frameBuffer = std::move(fbo);
}