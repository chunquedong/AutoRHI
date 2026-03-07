/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef VKBuffer_H_
#define VKBuffer_H_

#include "VKDevice.h"

namespace arhi {

class VKBuffer : public Buffer {
    VKDevice* device = nullptr;
public:
    uint64_t size;
    /// The Vulkan buffer object that holds the vertex data for the triangle.
    VkBuffer buffer = VK_NULL_HANDLE;

    /// The device memory allocated for the vertex buffer.
    VkDeviceMemory memory = VK_NULL_HANDLE;

    /// Vulkan Memory Allocator (VMA) allocation info for the vertex buffer.
    VmaAllocation allocation = VK_NULL_HANDLE;

    VmaAllocationInfo alloc_info{};

    ~VKBuffer();
    void init(VKDevice* device, const BufferDesc& desc);
    void setData(int offset, const void* data, int size) override;
};

}
#endif