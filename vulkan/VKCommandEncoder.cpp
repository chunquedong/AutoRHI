#include "VKCommandEncoder.h"
#include "VKBuffer.h"
#include "VKPipeline.h"
#include "VKTexture.h"

using namespace arhi;

void VKCommandEncoder::init(VKDevice* device, const CommandEncoderDesc* desc) {
    this->device = device;

	VkCommandBufferAllocateInfo cmd_buf_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = device->primary_command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1 };
	VK_CHECK(vkAllocateCommandBuffers(device->device, &cmd_buf_info, &commandBuffer));

	VkFenceCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT };
	VK_CHECK(vkCreateFence(device->device, &info, nullptr, &queue_submit_fence));
	vkResetFences(this->device->device, 1, &queue_submit_fence);

}

VKCommandEncoder::~VKCommandEncoder() {
	VkDevice device = this->device->device;

	cleanup();

	if (queue_submit_fence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device, queue_submit_fence, nullptr);

		queue_submit_fence = VK_NULL_HANDLE;
	}

	if (commandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(device, this->device->primary_command_pool, 1, &commandBuffer);

		commandBuffer = VK_NULL_HANDLE;
	}
}

bool VKCommandEncoder::beginPass(FrameBuffer* frameBuffer) {
	VKFrameBuffer* renderPass = dynamic_cast<VKFrameBuffer*>(frameBuffer);
	//framebuffers.push_back(renderPass);

	if (!isBegined) {
		// We will only submit this once before it's recycled.
		VkCommandBufferBeginInfo begin_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
		// Begin command recording
		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &begin_info));
	}

	// Set clear color values.
	VkClearValue clear_value{};

	VKTexture* textureView = nullptr;
	std::vector<VkImageView> colorAttachments;
	frameState = renderPass->frameState;
	for (auto& it : renderPass->desc.colorAttachments) {
		textureView = dynamic_cast<VKTexture*>(it.view.get());
		colorAttachments.push_back(textureView->textureView);
		clear_value.color = { it.clearValue[0], it.clearValue[1], it.clearValue[2], it.clearValue[3] };
	}

	if (!textureView) {
		ARHI_ERROR("Invalide colorAttachments\n");
		return false;
	}

	uint32_t viewWidth = textureView->desc.width;
	uint32_t viewHeight = textureView->desc.height;

	// Begin the render pass.
	VkRenderPassBeginInfo rp_begin{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass->renderPass,
		.framebuffer = renderPass->framebuffer,
		.renderArea = {.extent = {.width = viewWidth, .height = viewHeight}},
		.clearValueCount = 1,
		.pClearValues = &clear_value };
	// We will add draw commands in the same command buffer.
	vkCmdBeginRenderPass(commandBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport vp{
		.x = 0,
		.y = (float)viewHeight,
		.width = (float)viewWidth,
		.height = -(float)viewHeight,
		.minDepth = 0.0f,
		.maxDepth = 1.0f };
	// Set viewport dynamically
	vkCmdSetViewport(commandBuffer, 0, 1, &vp);

	VkRect2D scissor{.extent = {.width = viewWidth, .height = viewHeight} };
	// Set scissor dynamically
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    return true;
}

void VKCommandEncoder::endPass() {
    
	// Complete render pass.
	vkCmdEndRenderPass(commandBuffer);
}

void VKCommandEncoder::submit() {
	// Complete the command buffer.
	VK_CHECK(vkEndCommandBuffer(commandBuffer));

	//// Submit it to the queue with a release semaphore.
	//if (syncState->swapchain_release_semaphore == VK_NULL_HANDLE)
	//{
	//	VkSemaphoreCreateInfo semaphore_info{
	//		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	//	VK_CHECK(vkCreateSemaphore(device->device, &semaphore_info, nullptr, &syncState->swapchain_release_semaphore));
	//}
	if (frameState) {
		auto syncState = frameState;
		VkPipelineStageFlags wait_stage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &syncState->swapchain_acquire_semaphore,
			.pWaitDstStageMask = &wait_stage,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &syncState->swapchain_release_semaphore };
		// Submit command buffer to graphics queue
		VK_CHECK(vkQueueSubmit(device->queue, 1, &info, queue_submit_fence));
	}
	isBegined = false;
	frameState = nullptr;
}

void VKCommandEncoder::wait()
{
	if (queue_submit_fence != VK_NULL_HANDLE)
	{
		// Only wait if the fence is not already signaled
		VkResult result = vkWaitForFences(this->device->device, 1, &queue_submit_fence, true, UINT64_MAX);
		if (result == VK_SUCCESS) {
			vkResetFences(this->device->device, 1, &queue_submit_fence);
		}
	}
}

void VKCommandEncoder::reset() {
	cleanup();

	vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
}

void VKCommandEncoder::cleanup() {
	//framebuffers.clear();
}

void VKCommandEncoder::setPipeline(Pipeline* pipeline) {
    VKPipeline* wgPipeline = dynamic_cast<VKPipeline*>(pipeline);
	// Bind the graphics pipeline.
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, wgPipeline->pipeline);
    curPipeline = wgPipeline;
}

void VKCommandEncoder::setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) {
    VKBindingGroup* wgBindingGroup = dynamic_cast<VKBindingGroup*>(bindingGroup);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, wgBindingGroup->pipelineLayout, groupIndex, 1,
		&wgBindingGroup->descriptorSet, 0, nullptr);
}

void VKCommandEncoder::setIndexBuffer(Buffer* b, int offset, IndexFormat indexFormat) {
    VKBuffer* t = dynamic_cast<VKBuffer*>(b);
    //uint64_t usize = size == -1 ? Vk_WHOLE_SIZE : size;
    //wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, t->buffer, VkIndexFormat_Uint32, offset, usize);
	VkIndexType it = VK_INDEX_TYPE_UINT32;
	if (indexFormat == IndexFormat::Uint16) {
		it = VK_INDEX_TYPE_UINT16;
	}
	vkCmdBindIndexBuffer(commandBuffer, t->buffer, offset, it);
}

void VKCommandEncoder::setVertexBuffer(Buffer* b, int offset, int binding) {
    VKBuffer* t = dynamic_cast<VKBuffer*>(b);
    //uint64_t usize = size == -1 ? Vk_WHOLE_SIZE : size;
    //wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, binding, t->buffer, offset, usize);
	VkDeviceSize vkoffset = { (uint64_t)offset };
	vkCmdBindVertexBuffers(commandBuffer, binding, 1, &t->buffer, &vkoffset);
}

void VKCommandEncoder::drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) {
    //wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indices, instances, firstindex, baseVertex, firstinstance);
	vkCmdDrawIndexed(commandBuffer, indices, instances, firstindex, baseVertex, firstinstance);
}
void VKCommandEncoder::draw(uint32_t vertices, uint32_t instances, uint32_t firstvertex, uint32_t firstinstance) {
	vkCmdDraw(commandBuffer, vertices, instances, firstvertex, firstinstance);
}
void VKCommandEncoder::setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	VkRect2D scissor{
		.extent = {.width = width, .height = height} };
	// Set scissor dynamically
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VKCommandEncoder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
	VkViewport vp{
		.x = x,
		.y = height-y,
		.width = (width),
		.height = (-height),
		.minDepth = minDepth,
		.maxDepth = maxDepth };
	// Set viewport dynamically
	vkCmdSetViewport(commandBuffer, 0, 1, &vp);
}

void VKBindingGroup::init(VKDevice* device, BindingGroupDesc&& desc)
{
    //this->desc = *desc;
    VKPipeline* pipeline = dynamic_cast<VKPipeline*>(desc.pipeline);
	pipelineLayout = pipeline->pipelineLayout;
    
    //index sresource
    std::map<std::string, const BindingEntry*> resourceMap;
    for (auto it = desc.resources.begin(); it != desc.resources.end(); ++it) {
        const BindingEntry* entry = &(*it);
        resourceMap[entry->name] = entry;
    }
    
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = device->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
	allocInfo.pSetLayouts = &pipeline->descriptorSetLayouts[desc.bindingGroup];

	if (vkAllocateDescriptorSets(device->device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		ARHI_ERROR("failed to allocate descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> descriptorWrites{};
	for (auto it = pipeline->reflection.uniforms.begin(); it != pipeline->reflection.uniforms.end(); ++it) {
		const UniformVar& uniform = it->second;
		auto found = resourceMap.find(uniform.name);
		if (found == resourceMap.end()) {
			ARHI_ERROR("ERROR unknow binding resource: %s\n", uniform.name.c_str());
			continue;
		}
		int binding = uniform.binding + found->second->offset;
		Resource* resource = found->second->resource.get();

		if (VKTexture* tex = dynamic_cast<VKTexture*>(resource)) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = tex->textureView;
			//imageInfo.sampler = textureSampler;
			VkWriteDescriptorSet entry = {};
			entry.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			entry.dstSet = descriptorSet;
			entry.dstBinding = (uint32_t)binding;
			entry.dstArrayElement = 0;
			entry.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			entry.descriptorCount = 1;
			entry.pImageInfo = &imageInfo;

			descriptorWrites.push_back(entry);
		}
		else if (VKSampler* tex = dynamic_cast<VKSampler*>(resource)) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			//imageInfo.imageView = tex->textureView;
			imageInfo.sampler = tex->sampler;
			VkWriteDescriptorSet entry = {};
			entry.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			entry.dstSet = descriptorSet;
			entry.dstBinding = (uint32_t)binding;
			entry.dstArrayElement = 0;
			entry.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			entry.descriptorCount = 1;
			entry.pImageInfo = &imageInfo;

			descriptorWrites.push_back(entry);
		}
		else if (VKBuffer* buffer = dynamic_cast<VKBuffer*>(resource)) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer->buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = buffer->size;

			VkWriteDescriptorSet entry = {};
			entry.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			entry.dstSet = descriptorSet;
			entry.dstBinding = (uint32_t)binding;
			entry.dstArrayElement = 0;
			entry.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			entry.descriptorCount = 1;
			entry.pBufferInfo = &bufferInfo;
			descriptorWrites.push_back(entry);
		}
		else {
			ARHI_ERROR("ERROR unknow binding resource type\n");
		}
	}

	vkUpdateDescriptorSets(device->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	this->desc = std::move(desc);
}

VKBindingGroup::~VKBindingGroup()
{
    
}

VkRenderPass create_render_pass(VKDevice* device, const RenderPassDesc& desc)
{
	//TODO
	VkFormat swapchain_format = VK_FORMAT_R8G8B8A8_UNORM;

	VkAttachmentDescription attachment{
		.format = swapchain_format,        // Backbuffer format.
		.samples = VK_SAMPLE_COUNT_1_BIT,                      // Not multisampled.
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,                // When starting the frame, we want tiles to be cleared.
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,               // When ending the frame, we want tiles to be written out.
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,            // Don't care about stencil since we're not using it.
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,           // Don't care about stencil since we're not using it.
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,                  // The image layout will be undefined when the render pass begins.
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR             // After the render pass is complete, we will transition to PRESENT_SRC_KHR layout.
	};

	// We have one subpass. This subpass has one color attachment.
	// While executing this subpass, the attachment will be in attachment optimal layout.
	VkAttachmentReference color_ref = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	// We will end up with two transitions.
	// The first one happens right before we start subpass #0, where
	// UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
	// The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
	// will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
	VkSubpassDescription subpass{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_ref,
	};

	// Create a dependency to external events.
	// We need to wait for the WSI semaphore to signal.
	// Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
	// actually wait for the semaphore, so we must also wait for that pipeline stage.
	VkSubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	// Since we changed the image layout, we need to make the memory visible to
	// color attachment to modify.
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Finally, create the renderpass.
	VkRenderPassCreateInfo rp_info{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency };

	VkRenderPass renderPass;
	VK_CHECK(vkCreateRenderPass(device->device, &rp_info, nullptr, &renderPass));
	return renderPass;
}

bool VKFrameBuffer::init(VKDevice *device, RenderPassDesc&& desc) {
	this->device = device;

	VKTexture* textureView = nullptr;
	std::vector<VkImageView> colorAttachments;
	frameState = nullptr;
	for (auto& it : desc.colorAttachments) {
		textureView = dynamic_cast<VKTexture*>(it.view.get());
		if (textureView->frameState) {
			frameState = textureView->frameState;
		}
		colorAttachments.push_back(textureView->textureView);
	}

	if (!textureView) {
		ARHI_ERROR("Invalide colorAttachments\n");
		return false;
	}

	renderPass = create_render_pass(device, desc);

	uint32_t viewWidth = textureView->desc.width;
	uint32_t viewHeight = textureView->desc.height;


	// Build the framebuffer.
	VkFramebufferCreateInfo fb_info{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = renderPass,
		.attachmentCount = (uint32_t)colorAttachments.size(),
		.pAttachments = colorAttachments.data(),
		.width = viewWidth,
		.height = viewHeight,
		.layers = 1 };
	VkFramebuffer frameBuffer;
	VK_CHECK(vkCreateFramebuffer(device->device, &fb_info, nullptr, &frameBuffer));

	this->framebuffer = frameBuffer;
	this->desc = std::move(desc);
	return true;
}

VKFrameBuffer::~VKFrameBuffer() {
	if (framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(this->device->device, framebuffer, nullptr);
	}

	if (renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(this->device->device, renderPass, nullptr);
	}
}