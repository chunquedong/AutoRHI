#include "VKBuffer.h"

using namespace mrhi;

void VKBuffer::init(VKDevice* device, const BufferDesc& desc) {
    this->device = device;
    this->size = desc.size;

	VkBufferUsageFlags gltype = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if ((desc.usage & BufferDesc::Usage_Index) != 0) {
		gltype = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	else if ((desc.usage & BufferDesc::Usage_Uniform) != 0) {
		gltype = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}
	else if ((desc.usage & BufferDesc::Usage_Storage) != 0) {
		gltype = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

	VkBufferCreateInfo buffer_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = (VkDeviceSize)desc.size,
		.usage = gltype };

	// We use the Vulkan Memory Allocator to find a memory type that can be written and mapped from the host
	// On most setups this will return a memory type that resides in VRAM and is accessible from the host
	VmaAllocationCreateInfo buffer_alloc_ci{
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
		.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

	vmaCreateBuffer(device->vma_allocator, &buffer_info, &buffer_alloc_ci, &buffer, &allocation, &alloc_info);
}

void VKBuffer::setData(int offset, const void* data, int size) {
	VkDevice device = this->device->device;

	if (alloc_info.pMappedData)
	{
		memcpy(alloc_info.pMappedData, data, size);
	}
}

VKBuffer::~VKBuffer() {
	if (allocation != VK_NULL_HANDLE)
	{
		vmaDestroyBuffer(this->device->vma_allocator, buffer, allocation);
	}
}