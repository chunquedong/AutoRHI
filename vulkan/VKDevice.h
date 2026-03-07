/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef VKDevice_H_
#define VKDevice_H_

#include "rhi/GraphicsDevice.h"
#include "VKUtil.h"


namespace arhi {

class VKDevice : public GraphicsDevice {
public:
    /// The Vulkan instance.
    VkInstance instance = VK_NULL_HANDLE;

    /// The Vulkan physical device.
    VkPhysicalDevice gpu = VK_NULL_HANDLE;

    /// The Vulkan device.
    VkDevice device = VK_NULL_HANDLE;

    /// The Vulkan device queue.
    VkQueue queue = VK_NULL_HANDLE;

    VmaAllocator vma_allocator = VK_NULL_HANDLE;

    /// The queue family index where graphics work will be submitted.
    int32_t graphics_queue_index = -1;

    /// The surface we will render to.
    //VkSurfaceKHR surface = VK_NULL_HANDLE;

    /// The debug utility callback.
    VkDebugUtilsMessengerEXT debug_callback_obj = VK_NULL_HANDLE;

    /// A set of semaphores that can be reused.
    //std::vector<VkSemaphore> recycled_semaphores;

    VkCommandPool   primary_command_pool = VK_NULL_HANDLE;
    //VkFence         queue_submit_fence = VK_NULL_HANDLE;

    VkDescriptorPool descriptorPool;

    ~VKDevice();

    APtr<Surface> createSurface(const SurfaceDesc& desc) override;
    APtr<Pipeline> doCreatePipeline(const PipelineDesc& desc) override;
    APtr<Texture> createTexture(const TextureDesc& desc) override;
    APtr<Buffer> createBuffer(const BufferDesc& desc) override;
    APtr<CommandEncoder> createCommandEncoder(const CommandEncoderDesc& desc) override;
    APtr<Shader> createShader(const ShaderDesc& desc) override;
    APtr<Sampler> createSampler(const SamplerDesc& desc) override;
    APtr<BindingGroup> createBindingGroup(BindingGroupDesc&& desc) override;
    APtr<FrameBuffer> createFrameBuffer(RenderPassDesc&& desc) override;
    
    VkInstance initInstance(std::vector<const char*>& requiredExtensions, bool isDebug);
    void initDevice(VkSurfaceKHR surface);
    void waitIdle() override;
private:
    bool init_instance(std::vector<const char*>& requiredExtensions, bool isDebug);
    bool init_device(VkSurfaceKHR surface);
    void createDescriptorPool();
};

}
#endif