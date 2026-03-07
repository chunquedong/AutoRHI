#include "VKTexture.h"
#include "VKBuffer.h"
#include <cmath>

using namespace arhi;

uint32_t calculateMaxMipLevels(uint32_t width, uint32_t height, uint32_t depth) {
    return static_cast<uint32_t>(std::floor(std::log2(std::max({width, height, depth})))) + 1;
}

VkFormat getVkFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8Unorm:
        return VK_FORMAT_R8_UNORM;
    case TextureFormat::R8Snorm:
        return VK_FORMAT_R8_SNORM;
    case TextureFormat::R8Uint:
        return VK_FORMAT_R8_UINT;
    case TextureFormat::R8Sint:
        return VK_FORMAT_R8_SINT;
    case TextureFormat::R16Unorm:
        return VK_FORMAT_R16_UNORM;
    case TextureFormat::R16Snorm:
        return VK_FORMAT_R16_SNORM;
    case TextureFormat::R16Uint:
        return VK_FORMAT_R16_UINT;
    case TextureFormat::R16Sint:
        return VK_FORMAT_R16_SINT;
    case TextureFormat::R16Float:
        return VK_FORMAT_R16_SFLOAT;
    case TextureFormat::R32Float:
        return VK_FORMAT_R32_SFLOAT;
    case TextureFormat::R32Uint:
        return VK_FORMAT_R32_UINT;
    case TextureFormat::R32Sint:
        return VK_FORMAT_R32_SINT;
    case TextureFormat::RG8Unorm:
        return VK_FORMAT_R8G8_UNORM;
    case TextureFormat::RG8Snorm:
        return VK_FORMAT_R8G8_SNORM;
    case TextureFormat::RG8Uint:
        return VK_FORMAT_R8G8_UINT;
    case TextureFormat::RG8Sint:
        return VK_FORMAT_R8G8_SINT;
    case TextureFormat::RG16Unorm:
        return VK_FORMAT_R16G16_UNORM;
    case TextureFormat::RG16Snorm:
        return VK_FORMAT_R16G16_SNORM;
    case TextureFormat::RG16Uint:
        return VK_FORMAT_R16G16_UINT;
    case TextureFormat::RG16Sint:
        return VK_FORMAT_R16G16_SINT;
    case TextureFormat::RG16Float:
        return VK_FORMAT_R16G16_SFLOAT;
    case TextureFormat::RG32Float:
        return VK_FORMAT_R32G32_SFLOAT;
    case TextureFormat::RG32Uint:
        return VK_FORMAT_R32G32_UINT;
    case TextureFormat::RG32Sint:
        return VK_FORMAT_R32G32_SINT;
    case TextureFormat::RGBA8Unorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::RGBA8UnormSrgb:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case TextureFormat::RGBA8Snorm:
        return VK_FORMAT_R8G8B8A8_SNORM;
    case TextureFormat::RGBA8Uint:
        return VK_FORMAT_R8G8B8A8_UINT;
    case TextureFormat::RGBA8Sint:
        return VK_FORMAT_R8G8B8A8_SINT;
    case TextureFormat::BGRA8Unorm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::BGRA8UnormSrgb:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case TextureFormat::RGBA16Unorm:
        return VK_FORMAT_R16G16B16A16_UNORM;
    case TextureFormat::RGBA16Snorm:
        return VK_FORMAT_R16G16B16A16_SNORM;
    case TextureFormat::RGBA16Uint:
        return VK_FORMAT_R16G16B16A16_UINT;
    case TextureFormat::RGBA16Sint:
        return VK_FORMAT_R16G16B16A16_SINT;
    case TextureFormat::RGBA16Float:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case TextureFormat::RGBA32Float:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case TextureFormat::RGBA32Uint:
        return VK_FORMAT_R32G32B32A32_UINT;
    case TextureFormat::RGBA32Sint:
        return VK_FORMAT_R32G32B32A32_SINT;
    case TextureFormat::RGB10A2Uint:
        return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case TextureFormat::RGB10A2Unorm:
        return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case TextureFormat::RG11B10Ufloat:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    // case TextureFormat::RGB9E5Ufloat:
    //     return VK_FORMAT_R9G9B9E5_SHAREDEXP;
    case TextureFormat::Stencil8:
        return VK_FORMAT_S8_UINT;
    case TextureFormat::Depth16Unorm:
        return VK_FORMAT_D16_UNORM;
    case TextureFormat::Depth24Plus:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case TextureFormat::Depth24PlusStencil8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case TextureFormat::Depth32Float:
        return VK_FORMAT_D32_SFLOAT;
    case TextureFormat::Depth32FloatStencil8:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    default:
        ARHI_ERROR("ERROR: unsupport texture format %d\n", format);
        return VK_FORMAT_R8G8B8A8_UNORM;
    }
}

uint32_t findMemoryType(VKDevice* device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->gpu, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    ARHI_ERROR("failed to find suitable memory type!\n");
    abort();
}
void createBuffer(VKDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        ARHI_ERROR("failed to create buffer!\n");
        abort();
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(device, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        ARHI_ERROR("failed to allocate buffer memory!\n");
        abort();
    }

    vkBindBufferMemory(device->device, buffer, bufferMemory, 0);
}

VkCommandBuffer beginSingleTimeCommands(VKDevice* device) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device->primary_command_pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(VKDevice* device, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->queue);

    vkFreeCommandBuffers(device->device, device->primary_command_pool, 1, &commandBuffer);
}

void generateMipmaps(VKDevice* device, VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t layerCount, uint32_t mipLevels) {
    // Check if linear filtering is supported
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(device->gpu, format, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        ARHI_ERROR("texture format does not support linear filtering!");
        return;
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = width;
    int32_t mipHeight = height;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = layerCount;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = layerCount;

        vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    // Handle last mip level
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(device, commandBuffer);
}


void createImage(VKDevice* device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t arrayLayers, VkImageType imageType, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = imageType;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        ARHI_ERROR("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(device, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device->device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        ARHI_ERROR("failed to allocate image memory!");
    }

    vkBindImageMemory(device->device, image, imageMemory, 0);
}

void transitionImageLayout(VKDevice* device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else {
        ARHI_ERROR("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(device, commandBuffer);
}

void copyBufferToImage(VKDevice* device, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevel, uint32_t baseArrayLayer, uint32_t layerCount) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = mipLevel;
    region.imageSubresource.baseArrayLayer = baseArrayLayer;
    region.imageSubresource.layerCount = layerCount;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(device, commandBuffer);
}
VkImageView createImageView(VKDevice* device, VkImage image, VkFormat format, VkImageViewType viewType, uint32_t mipLevels, uint32_t layerCount) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;

    VkImageView imageView;
    if (vkCreateImageView(device->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        ARHI_ERROR("failed to create texture image view!");
    }

    return imageView;
}
void VKTexture::init(VKDevice* adevice, const TextureDesc* desc) {
    VkDevice device = adevice->device;
    this->desc = *desc;
    this->device = adevice;
    
    uint32_t texWidth = desc->width;
    uint32_t texHeight = desc->height;
    uint32_t mipLevels = desc->mipLevelCount;
    uint32_t arrayLayers = 1;
    VkImageType imageType = VK_IMAGE_TYPE_2D;
    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    VkFormat format = getVkFormat(desc->format);

    // Set parameters based on texture type
    switch (desc->type) {
    case TextureType::Cube:
        arrayLayers = 6; // Cubemap has 6 faces
        viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        break;
    case TextureType::_2DArray:
        arrayLayers = desc->depthOrArrayLayers;
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        break;
    case TextureType::_3D:
        imageType = VK_IMAGE_TYPE_3D;
        viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    case TextureType::_1D:
        imageType = VK_IMAGE_TYPE_1D;
        viewType = VK_IMAGE_VIEW_TYPE_1D;
        break;
    case TextureType::_2D:
    default:
        break;
    }

    // Ensure mipLevels is at least 1
    if (mipLevels == 0) {
        mipLevels = 1;
    }
    
    // Calculate maximum possible mip levels based on texture size
    uint32_t maxMipLevels = calculateMaxMipLevels(texWidth, texHeight, 1);
    // Check if requested mip levels exceed maximum possible
    if (mipLevels > maxMipLevels) {
        ARHI_ERROR("ERROR: Requested mip levels (%u) exceed maximum possible (%u) for texture size %ux%u!\n", 
                  mipLevels, maxMipLevels, texWidth, texHeight);
    }

    // Create image
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    if (mipLevels > 1) {
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // For mipmap generation
    }

    createImage(adevice, texWidth, texHeight, mipLevels, arrayLayers, imageType, format, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

    // Create image view
    textureView = createImageView(adevice, textureImage, format, viewType, mipLevels, arrayLayers);
}

void VKTexture::setData(const void* textureData, int mipLevel, int depthOrArrayLayers) {
    if (!textureImage) {
        ARHI_ERROR("Null Texture\n");
        abort();
        return;
    }

    // Calculate current mip level width and height
    uint32_t texWidth = desc.width >> mipLevel;
    uint32_t texHeight = desc.height >> mipLevel;
    if (texWidth == 0) texWidth = 1;
    if (texHeight == 0) texHeight = 1;

    // Calculate bytes per pixel using getBytePerPixel function
    uint32_t bpp = getBytePerPixel(desc.format);
    uint32_t layerCount = depthOrArrayLayers;

    // Ensure layerCount is at least 1
    if (layerCount == 0) {
        layerCount = 1;
    }

    // For Cubemap, ensure layerCount is 6
    if (desc.type == TextureType::Cube) {
        layerCount = 6;
    }

    uint32_t size = texWidth * texHeight * bpp * layerCount;

VkBuffer stagingBuffer = VK_NULL_HANDLE;
#ifdef NO_VMA
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    createBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    
    if (stagingBuffer != VK_NULL_HANDLE) {
        void* data;
        vkMapMemory(device->device, stagingBufferMemory, 0, size, 0, &data);
        memcpy(data, textureData, static_cast<size_t>(size));
        vkUnmapMemory(device->device, stagingBufferMemory);
    } else {
        ARHI_ERROR("failed to create staging buffer in NO_VMA mode!\n");
        return;
    }
#else
    /// Vulkan Memory Allocator (VMA) allocation info for the vertex buffer.
    VmaAllocation allocation = VK_NULL_HANDLE;

    VkBufferCreateInfo buffer_info{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = (VkDeviceSize)size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT };
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // We use the Vulkan Memory Allocator to find a memory type that can be written and mapped from the host
    // On most setups this will return a memory type that resides in VRAM and is accessible from the host
    VmaAllocationCreateInfo buffer_alloc_ci{
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VmaAllocationInfo alloc_info{};
    VkResult result = vmaCreateBuffer(device->vma_allocator, &buffer_info, &buffer_alloc_ci, &stagingBuffer, &allocation, &alloc_info);
    if (result != VK_SUCCESS) {
        ARHI_ERROR("failed to create staging buffer! result: %d\n", result);
        return;
    }

    if (alloc_info.pMappedData && textureData)
    {
        memcpy(alloc_info.pMappedData, textureData, size);
    }
#endif

    VkFormat format = getVkFormat(desc.format);
    uint32_t totalLayers = (desc.type == TextureType::Cube) ? 6 : desc.depthOrArrayLayers;

    // Check if staging buffer was created successfully
    if (stagingBuffer == VK_NULL_HANDLE) {
        ARHI_ERROR("ERROR: Staging buffer creation failed!\n");
        return;
    }

    // Transition layout to transfer destination
    transitionImageLayout(device, textureImage, format, currentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, desc.mipLevelCount, totalLayers);
    currentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    // Copy data to image
    copyBufferToImage(device, stagingBuffer, textureImage, texWidth, texHeight, mipLevel, 0, layerCount);

    // Generate mipmaps (if needed)
    if (mipLevel == 0 && desc.mipLevelCount > 1) {
        generateMipmaps(device, textureImage, format, desc.width, desc.height, totalLayers, desc.mipLevelCount);
        currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    } else {
        // Transition layout to shader read
        transitionImageLayout(device, textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, desc.mipLevelCount, totalLayers);
        currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

#ifdef NO_VMA
    vkDestroyBuffer(device->device, stagingBuffer, nullptr);
    vkFreeMemory(device->device, stagingBufferMemory, nullptr);
#else
    if (allocation != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(this->device->vma_allocator, stagingBuffer, allocation);
    }
#endif
}

VKTexture::~VKTexture() {
    VkDevice device = this->device->device;

    // Check if it's a surface
    if (frameState) {
        return;
    }

    if (textureView)
        vkDestroyImageView(device, textureView, nullptr);

    if (textureImage)
        vkDestroyImage(device, textureImage, nullptr);

    if (textureImageMemory)
        vkFreeMemory(device, textureImageMemory, nullptr);
}

VKSampler::~VKSampler()
{
    vkDestroySampler(device->device, sampler, nullptr);
}

VkFilter filterToVk(SamplerFilterMode filter) {
    switch (filter) {
    case SamplerFilterMode::Nearest:
        return VK_FILTER_NEAREST;
    case SamplerFilterMode::Linear:
        return VK_FILTER_LINEAR;
    default:
        return VK_FILTER_LINEAR;
    }
}

VkSamplerAddressMode wrapModeToVk(SamplerAddressMode mode) {
    switch (mode) {
    case SamplerAddressMode::ClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case SamplerAddressMode::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case SamplerAddressMode::MirrorRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    default:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

VkSamplerMipmapMode mipmapFilterToVk(SamplerFilterMode filter) {
    switch (filter) {
    case SamplerFilterMode::Nearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case SamplerFilterMode::Linear:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

void VKSampler::init(VKDevice* device, const SamplerDesc* desc) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device->gpu, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filterToVk(desc->magFilter);
    samplerInfo.minFilter = filterToVk(desc->minFilter);
    samplerInfo.addressModeU = wrapModeToVk(desc->addressModeU);
    samplerInfo.addressModeV = wrapModeToVk(desc->addressModeV);
    samplerInfo.addressModeW = wrapModeToVk(desc->addressModeW);
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = mipmapFilterToVk(desc->mipmapFilter);
    samplerInfo.minLod = desc->lodMinClamp;
    samplerInfo.maxLod = desc->lodMaxClamp;

    VkSampler textureSampler;
    if (vkCreateSampler(device->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        ARHI_ERROR("failed to create texture sampler!");
    }
    this->device = device;
    this->sampler = textureSampler;
}

