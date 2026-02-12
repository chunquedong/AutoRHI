#include "VKSurface.h"
#include "VKTexture.h"
#include "VKDevice.h"
#include "VKCommandEncoder.h"

using namespace arhi;

VKSurface::~VKSurface() {
	VkDevice device = this->device->device;
	//for (auto& framebuffer : swapchain_framebuffers)
	//{
	//	vkDestroyFramebuffer(device, framebuffer, nullptr);
	//}

	for (auto& per_frame : per_frame)
	{
		teardown_per_frame(per_frame);
	}

	per_frame.clear();

	for (auto semaphore : recycled_semaphores)
	{
		vkDestroySemaphore(device, semaphore, nullptr);
	}

	for (VkImageView image_view : swapchain_image_views)
	{
		vkDestroyImageView(device, image_view, nullptr);
	}

	if (swapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}

	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(this->device->instance, surface, nullptr);
	}
}

VkSurfaceFormatKHR select_surface_format(VkPhysicalDevice gpu, VkSurfaceKHR surface, std::vector<VkFormat> const& preferred_formats)
{
	uint32_t surface_format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, nullptr);
	assert(0 < surface_format_count);
	std::vector<VkSurfaceFormatKHR> supported_surface_formats(surface_format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, supported_surface_formats.data());

	for (auto& format : preferred_formats) {
		for (VkSurfaceFormatKHR& surfaceFormat : supported_surface_formats) {
			if (surfaceFormat.format == format) {
				return surfaceFormat;
			}
		}
	}

	// We use the first supported format as a fallback in case none of the preferred formats is available
	return supported_surface_formats[0];
}


void VKSurface::init_per_frame(FrameState& per_frame)
{
	/*VkFenceCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT };
	VK_CHECK(vkCreateFence(device->device, &info, nullptr, &per_frame.queue_submit_fence));*/

	//VkCommandPoolCreateInfo cmd_pool_info{
	//	.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	//	.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
	//	.queueFamilyIndex = static_cast<uint32_t>(this->graphics_queue_index) };
	//VK_CHECK(vkCreateCommandPool(device->device, &cmd_pool_info, nullptr, &per_frame.primary_command_pool));

	//VkCommandBufferAllocateInfo cmd_buf_info{
	//	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	//	.commandPool = per_frame.primary_command_pool,
	//	.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	//	.commandBufferCount = 1 };
	//VK_CHECK(vkAllocateCommandBuffers(device->device, &cmd_buf_info, &per_frame.primary_command_buffer));
}

void VKSurface::teardown_per_frame(FrameState& per_frame)
{
	/*if (per_frame.queue_submit_fence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device->device, per_frame.queue_submit_fence, nullptr);

		per_frame.queue_submit_fence = VK_NULL_HANDLE;
	}*/
	/*
	if (per_frame.primary_command_buffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(device->device, per_frame.primary_command_pool, 1, &per_frame.primary_command_buffer);

		per_frame.primary_command_buffer = VK_NULL_HANDLE;
	}

	if (per_frame.primary_command_pool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device->device, per_frame.primary_command_pool, nullptr);

		per_frame.primary_command_pool = VK_NULL_HANDLE;
	}*/

	if (per_frame.swapchain_acquire_semaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(device->device, per_frame.swapchain_acquire_semaphore, nullptr);

		per_frame.swapchain_acquire_semaphore = VK_NULL_HANDLE;
	}

	if (per_frame.swapchain_release_semaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(device->device, per_frame.swapchain_release_semaphore, nullptr);

		per_frame.swapchain_release_semaphore = VK_NULL_HANDLE;
	}

	if (per_frame.commandEncoder)
	{
		delete per_frame.commandEncoder;
		per_frame.commandEncoder = nullptr;
	}

	if (per_frame.frameBuffer)
	{
		delete per_frame.frameBuffer;
		per_frame.frameBuffer = nullptr;
	}

	if (per_frame.textureView)
	{
		delete per_frame.textureView;
		per_frame.textureView = nullptr;
	}
}

bool VKSurface::init_swapchain()
{
	VkSurfaceCapabilitiesKHR surface_properties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->gpu, this->surface, &surface_properties));

	VkSurfaceFormatKHR format = select_surface_format(device->gpu, this->surface, { VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_SRGB });

	VkExtent2D swapchain_size{};
	if (surface_properties.currentExtent.width == 0xFFFFFFFF)
	{
		swapchain_size.width = this->swapchain_dimensions.width;
		swapchain_size.height = this->swapchain_dimensions.height;
	}
	else
	{
		swapchain_size = surface_properties.currentExtent;
	}

	// FIFO must be supported by all implementations.
	VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;

	// Determine the number of VkImage's to use in the swapchain.
	// Ideally, we desire to own 1 image at a time, the rest of the images can
	// either be rendered to and/or being queued up for display.
	uint32_t desired_swapchain_images = surface_properties.minImageCount + 1;
	if ((surface_properties.maxImageCount > 0) && (desired_swapchain_images > surface_properties.maxImageCount))
	{
		// Application must settle for fewer images than desired.
		desired_swapchain_images = surface_properties.maxImageCount;
	}

	// Figure out a suitable surface transform.
	VkSurfaceTransformFlagBitsKHR pre_transform;
	if (surface_properties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		pre_transform = surface_properties.currentTransform;
	}

	VkSwapchainKHR old_swapchain = this->swapchain;

	// Find a supported composite type.
	VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}
	else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	}
	else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	}
	else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	}

	VkSwapchainCreateInfoKHR info{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = this->surface,
		.minImageCount = desired_swapchain_images,
		.imageFormat = format.format,
		.imageColorSpace = format.colorSpace,
		.imageExtent = swapchain_size,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = pre_transform,
		.compositeAlpha = composite,
		.presentMode = swapchain_present_mode,
		.clipped = true,
		.oldSwapchain = old_swapchain };

	VK_CHECK(vkCreateSwapchainKHR(device->device, &info, nullptr, &this->swapchain));

	if (old_swapchain != VK_NULL_HANDLE)
	{
		for (VkImageView image_view : this->swapchain_image_views)
		{
			vkDestroyImageView(device->device, image_view, nullptr);
		}

		for (auto& per_frame : this->per_frame)
		{
			teardown_per_frame(per_frame);
		}

		this->swapchain_image_views.clear();

		vkDestroySwapchainKHR(device->device, old_swapchain, nullptr);
	}

	this->swapchain_dimensions = { swapchain_size.width, swapchain_size.height, format.format };

	uint32_t image_count;
	VK_CHECK(vkGetSwapchainImagesKHR(device->device, this->swapchain, &image_count, nullptr));

	/// The swapchain images.
	std::vector<VkImage> swapchain_images(image_count);
	VK_CHECK(vkGetSwapchainImagesKHR(device->device, this->swapchain, &image_count, swapchain_images.data()));

	// Initialize per-frame resources.
	// Every swapchain image has its own command pool and fence manager.
	// This makes it very easy to keep track of when we can reset command buffers and such.
	this->per_frame.clear();
	this->per_frame.resize(image_count);

	for (size_t i = 0; i < image_count; i++)
	{
		init_per_frame(this->per_frame[i]);
	}

	for (size_t i = 0; i < image_count; i++)
	{
		// Create an image view which we can render into.
		VkImageViewCreateInfo view_info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapchain_images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = this->swapchain_dimensions.format,
			.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1} };

		VkImageView image_view;
		VK_CHECK(vkCreateImageView(device->device, &view_info, nullptr, &image_view));

		this->swapchain_image_views.push_back(image_view);
	}
	return true;
}

void VKSurface::init(VKDevice* device, const SurfaceDesc* desc) {
	this->swapchain_dimensions.width = desc->width;
	this->swapchain_dimensions.height = desc->height;

    this->surface = (VkSurfaceKHR)desc->surfaceChain;
	this->graphics_queue_index = device->graphics_queue_index;
    this->device = device;
    this->desc = *desc;

	device->initDevice(this->surface);
	init_swapchain();
}

void VKSurface::resize(int w, int h) {
	if (this->device->device == VK_NULL_HANDLE)
	{
		return;
	}

	VkSurfaceCapabilitiesKHR surface_properties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->gpu, surface, &surface_properties));

	// Only rebuild the swapchain if the dimensions have changed
	if (surface_properties.currentExtent.width == swapchain_dimensions.width &&
		surface_properties.currentExtent.height == swapchain_dimensions.height)
	{
		return;
	}

	vkDeviceWaitIdle(this->device->device);

	/*for (auto& framebuffer : swapchain_framebuffers)
	{
		vkDestroyFramebuffer(this->device->device, framebuffer, nullptr);
	}*/

	init_swapchain();
	return;
}

/**
 * @brief Acquires an image from the swapchain.
 * @param[out] image The swapchain index for the acquired image.
 * @returns Vulkan result code
 */
VkResult VKSurface::acquire_next_image()
{
	VkSemaphore acquire_semaphore;
	if (recycled_semaphores.empty())
	{
		VkSemaphoreCreateInfo info = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VK_CHECK(vkCreateSemaphore(this->device->device, &info, nullptr, &acquire_semaphore));
	}
	else
	{
		acquire_semaphore = recycled_semaphores.back();
		recycled_semaphores.pop_back();
	}

	uint32_t image = current_frame_index;
	VkResult res = vkAcquireNextImageKHR(this->device->device, swapchain, UINT64_MAX, acquire_semaphore, VK_NULL_HANDLE, &image);
	current_frame_index = image;

	if (res != VK_SUCCESS)
	{
		recycled_semaphores.push_back(acquire_semaphore);
		return res;
	}

	//PerFrame* syncState = &per_frame[current_frame_index];
	// If we have outstanding fences for this swapchain image, wait for them to complete first.
	// After begin frame returns, it is safe to reuse or delete resources which
	// were used previously.
	//
	// We wait for fences which completes N frames earlier, so we do not stall,
	// waiting for all GPU work to complete before this returns.
	// Normally, this doesn't really block at all,
	// since we're waiting for old frames to have been completed, but just in case.
	//if (syncState->queue_submit_fence != VK_NULL_HANDLE)
	//{
	//	vkWaitForFences(this->device->device, 1, &syncState->queue_submit_fence, true, UINT64_MAX);
	//	vkResetFences(this->device->device, 1, &syncState->queue_submit_fence);
	//}

	/*if (device->primary_command_pool != VK_NULL_HANDLE)
	{
		vkResetCommandPool(this->device->device, device->primary_command_pool, 0);
	}*/
	auto commandEncoder = per_frame[current_frame_index].commandEncoder;
	if (commandEncoder) {
		commandEncoder->wait();
		commandEncoder->reset();
	}
	// Recycle the old semaphore back into the semaphore manager.
	VkSemaphore old_semaphore = per_frame[current_frame_index].swapchain_acquire_semaphore;

	if (old_semaphore != VK_NULL_HANDLE)
	{
		recycled_semaphores.push_back(old_semaphore);
	}

	per_frame[current_frame_index].swapchain_acquire_semaphore = acquire_semaphore;

	// Submit it to the queue with a release semaphore.
	if (per_frame[current_frame_index].swapchain_release_semaphore == VK_NULL_HANDLE)
	{
		VkSemaphoreCreateInfo semaphore_info{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VK_CHECK(vkCreateSemaphore(this->device->device, &semaphore_info, nullptr, &per_frame[current_frame_index].swapchain_release_semaphore));
	}

	return VK_SUCCESS;
}

bool VKSurface::nextImage() {
	auto res = acquire_next_image();

	// Handle outdated error in acquire.
	if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		resize(swapchain_dimensions.width, swapchain_dimensions.height);
		res = acquire_next_image();
	}

	if (res != VK_SUCCESS)
	{
		vkQueueWaitIdle(device->queue);
		return false;
	}


	VKCommandEncoder* cmd = per_frame[current_frame_index].commandEncoder;
	if (!cmd) {
		cmd = dynamic_cast<VKCommandEncoder*>(device->createCommandEncoder(CommandEncoderDesc{}));
		per_frame[current_frame_index].commandEncoder = cmd;
	}

	return true;
}

void VKSurface::present() {
	VkPresentInfoKHR present{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &per_frame[current_frame_index].swapchain_release_semaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &current_frame_index,
	};
	// Present swapchain image
	auto res = vkQueuePresentKHR(device->queue, &present);
	// Handle Outdated error in present.
	if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		resize(swapchain_dimensions.width, swapchain_dimensions.height);
	}
	else if (res != VK_SUCCESS)
	{
		MGP_ERROR("Failed to present swapchain image.\n");
	}
}

Texture* VKSurface::getCurTextureView() {
	VKTexture* tex = per_frame[current_frame_index].textureView;
	if (!tex) {
		tex = new VKTexture();
		tex->device = device;
		tex->textureView = swapchain_image_views[current_frame_index];
		//tex->framebuffer = framebuffer;
		tex->desc.width = swapchain_dimensions.width;
		tex->desc.height = swapchain_dimensions.height;
		//tex->desc.format = swapchain_dimensions.format;
		tex->frameState = &per_frame[current_frame_index];
		per_frame[current_frame_index].textureView = tex;
	}
	return tex;
}
CommandEncoder* VKSurface::getCurCommandEncoder() {
	VKCommandEncoder* cmd = per_frame[current_frame_index].commandEncoder;
	if (!cmd) {
		cmd = dynamic_cast<VKCommandEncoder*>(device->createCommandEncoder(CommandEncoderDesc{}));
		per_frame[current_frame_index].commandEncoder = cmd;
	}
	return cmd;
}
FrameBuffer* VKSurface::getCurFrameBuffer() {
	return per_frame[current_frame_index].frameBuffer;
}
void VKSurface::cacheFrameBuffer(FrameBuffer* fbo) {
	if (per_frame[current_frame_index].frameBuffer) {
		delete per_frame[current_frame_index].frameBuffer;
	}
	per_frame[current_frame_index].frameBuffer = dynamic_cast<VKFrameBuffer*>(fbo);
}