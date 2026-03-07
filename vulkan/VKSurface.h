/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef VKSurface_H_
#define VKSurface_H_

#include "VKDevice.h"

namespace arhi {

class VKCommandEncoder;
class VKTexture;
class VKFrameBuffer;

struct FrameState {
    APtr<VKCommandEncoder> commandEncoder = nullptr;
    VkSemaphore     swapchain_acquire_semaphore = VK_NULL_HANDLE;
    VkSemaphore     swapchain_release_semaphore = VK_NULL_HANDLE;
    APtr<VKTexture> textureView = nullptr;
    APtr<VKFrameBuffer> frameBuffer = nullptr;
};

class VKSurface : public Surface {
    VKDevice *device = nullptr;
    SurfaceDesc desc;
    int32_t graphics_queue_index = -1;
    uint32_t current_frame_index = 0;
public:

    /**
    * @brief Swapchain state
    */
    struct SwapchainDimensions
    {
        /// Width of the swapchain.
        uint32_t width = 0;

        /// Height of the swapchain.
        uint32_t height = 0;

        /// Pixel format of the swapchain.
        VkFormat format = VK_FORMAT_UNDEFINED;
    };

    /// The swapchain.
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    /// The swapchain dimensions.
    SwapchainDimensions swapchain_dimensions;

    /// The surface we will render to.
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    /// The image view for each swapchain image.
    std::vector<VkImageView> swapchain_image_views;

    /// The framebuffer for each swapchain image view.
    //std::vector<VkFramebuffer> swapchain_framebuffers;

    /// A set of per-frame data.
    std::vector<FrameState> per_frame;

    /// A set of semaphores that can be reused.
    std::vector<VkSemaphore> recycled_semaphores;

    ~VKSurface();
    void init(VKDevice* device, const SurfaceDesc* desc);
    void resize(int w, int h) override;

    bool nextImage() override;

    void present() override;

    APtr<Texture> getCurTextureView() override;
    CommandEncoder* getCurCommandEncoder() override;
    APtr<FrameBuffer> getCurFrameBuffer() override;
    void cacheFrameBuffer(APtr<FrameBuffer> fbo) override;

private:
    bool init_swapchain();
    void init_per_frame(FrameState& per_frame);
    void teardown_per_frame(FrameState& per_frame);
    VkResult acquire_next_image();
    //void init_framebuffers();
};

}
#endif