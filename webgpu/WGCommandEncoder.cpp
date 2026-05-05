#include "WGCommandEncoder.h"
#include "WGBuffer.h"
#include "WGPipeline.h"
#include "WGTexture.h"

using namespace arhi;

void WGCommandEncoder::init(WGDevice* device, const CommandEncoderDesc* desc) {
    this->device = device;
    commandEncoder = wgpuDeviceCreateCommandEncoder(device->device, NULL);
}

WGCommandEncoder::~WGCommandEncoder() {
    wgpuCommandEncoderRelease(commandEncoder);
    commandEncoder = NULL;
}

bool WGCommandEncoder::beginPass(FrameBuffer* frameBuffer) {
    WGFrameBuffer* fb = dynamic_cast<WGFrameBuffer*>(frameBuffer);
    return beginPass(fb->desc);
}

bool WGCommandEncoder::beginPass(const RenderPassDesc& desc) {

    std::vector<WGPURenderPassColorAttachment> colorAttachments;
    for (auto& it : desc.colorAttachments) {

        WGTexture* textureView = dynamic_cast<WGTexture*>(it.view.get());

        WGPURenderPassColorAttachment colorAttachment = {
            .view = textureView->textureView,
            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
            .resolveTarget = NULL,
            .loadOp = (WGPULoadOp)it.loadOp,
            .storeOp = (WGPUStoreOp)it.storeOp,
            .clearValue = {it.clearValue[0], it.clearValue[1], it.clearValue[2], it.clearValue[3]},
        };
        colorAttachments.push_back(colorAttachment);
    }

    WGPURenderPassDescriptor passDescritpr = {
        .label = {.data = desc.label.c_str(), .length = desc.label.size()},
        .colorAttachmentCount = colorAttachments.size(),
        .colorAttachments = colorAttachments.data(),
    };

    WGPURenderPassDepthStencilAttachment depthStencelAttachment = {};
    if (desc.depthStencilAttachment) {
        depthStencelAttachment = {
            .view = dynamic_cast<WGTexture*>(desc.depthStencilAttachment->view.get())->textureView,
            .depthLoadOp = (WGPULoadOp)desc.depthStencilAttachment->depthLoadOp,
            .depthStoreOp = (WGPUStoreOp)desc.depthStencilAttachment->depthStoreOp,
            .depthClearValue = desc.depthStencilAttachment->depthClearValue,
            .depthReadOnly = desc.depthStencilAttachment->depthReadOnly,
            .stencilLoadOp = (WGPULoadOp)desc.depthStencilAttachment->stencilLoadOp,
            .stencilStoreOp = (WGPUStoreOp)desc.depthStencilAttachment->stencilStoreOp,
            .stencilClearValue = (uint32_t)desc.depthStencilAttachment->stencilClearValue,
            .stencilReadOnly = (uint32_t)desc.depthStencilAttachment->stencilReadOnly,
        };
        passDescritpr.depthStencilAttachment = &depthStencelAttachment;
    }

    renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &passDescritpr);

    return true;
}

void WGCommandEncoder::endPass() {
    wgpuRenderPassEncoderEnd(renderPassEncoder);
    wgpuRenderPassEncoderRelease(renderPassEncoder);
    renderPassEncoder = NULL;
}

void WGCommandEncoder::submit() {
    WGPUCommandBuffer cbuffer = wgpuCommandEncoderFinish(commandEncoder, NULL);

    // Submit.
    wgpuQueueSubmit(device->queue, 1, &cbuffer);

    wgpuCommandBufferRelease(cbuffer);
}

void WGCommandEncoder::setPipeline(Pipeline* pipeline) {
    WGPipeline* wgPipeline = dynamic_cast<WGPipeline*>(pipeline);
    wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgPipeline->pipeline);
    curPipeline = wgPipeline;
}

void WGCommandEncoder::setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) {
    WGBindingGroup* wgBindingGroup = dynamic_cast<WGBindingGroup*>(bindingGroup);
    wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, groupIndex, wgBindingGroup->bindGroup, 0, NULL);
}

void WGCommandEncoder::setIndexBuffer(Buffer* b, int offset, IndexFormat indexFormat) {
    WGBuffer* t = dynamic_cast<WGBuffer*>(b);
    wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, t->buffer, (WGPUIndexFormat)indexFormat, offset, WGPU_WHOLE_SIZE);
}

void WGCommandEncoder::setVertexBuffer(Buffer* b, int offset, int binding) {
    WGBuffer* t = dynamic_cast<WGBuffer*>(b);
    wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, binding, t->buffer, offset, WGPU_WHOLE_SIZE);
}

void WGCommandEncoder::drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) {
    wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indices, instances, firstindex, baseVertex, firstinstance);
}
void WGCommandEncoder::draw(uint32_t vertices, uint32_t instances, uint32_t firstvertex, uint32_t firstinstance) {
    wgpuRenderPassEncoderDraw(renderPassEncoder, vertices, instances, firstvertex, firstinstance);
}
void WGCommandEncoder::setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    wgpuRenderPassEncoderSetScissorRect(renderPassEncoder, x, y, width, height);
}

void WGCommandEncoder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
    wgpuRenderPassEncoderSetViewport(renderPassEncoder, x, y, width, height, minDepth, maxDepth);
}

void WGBindingGroup::init(WGDevice* device, BindingGroupDesc&& desc)
{
    //this->desc = *desc;
    WGPipeline* pipeline = dynamic_cast<WGPipeline*>(desc.pipeline);
    
    //index sresource
    std::map<std::string, const BindingEntry*> resourceMap;
    for (auto it = desc.resources.begin(); it != desc.resources.end(); ++it) {
        const BindingEntry* entry = &(*it);
        resourceMap[entry->name] = entry;
    }

    //init bindingGroup by uniform order
    std::vector<WGPUBindGroupEntry> bindGroupEntries;
    for (auto it = pipeline->reflection.uniforms.begin(); it != pipeline->reflection.uniforms.end(); ++it) {
        const UniformVar& uniform = it->second;
        auto found = resourceMap.find(uniform.name);
        if (found == resourceMap.end()) {
            ARHI_ERROR("ERROR unknow binding resource: %s\n", uniform.name.c_str());
            continue;
        }
        int binding = uniform.binding + found->second->offset;
        Resource* resource = found->second->resource.get();

        if (WGTexture* tex = dynamic_cast<WGTexture*>(resource)) {
            WGPUBindGroupEntry entry = {
                    .binding = (uint32_t)binding,
                    .textureView = tex->textureView,
            };
            bindGroupEntries.push_back(entry);
        }
        else if (WGSampler* tex = dynamic_cast<WGSampler*>(resource)) {
            WGPUBindGroupEntry entry = {
                .binding = (uint32_t)binding,
                .sampler = tex->sampler,
            };
            bindGroupEntries.push_back(entry);
        }
        else if (WGBuffer* buffer = dynamic_cast<WGBuffer*>(resource)) {
            WGPUBindGroupEntry entry = {
                .binding = (uint32_t)binding,
                .buffer = buffer->buffer,
                .size = buffer->size,
            };
            bindGroupEntries.push_back(entry);
        }
        else {
            ARHI_ERROR("ERROR unknow binding resource type\n");
        }
    }

    //mapping to WGPU
    WGPUBindGroupDescriptor bindGroupDesc = {
        .layout = pipeline->bindGroupLayoutList.at(desc.bindingGroup),
        .entryCount = bindGroupEntries.size(),
        .entries = bindGroupEntries.data()
    };
    WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(device->device, &bindGroupDesc);
    bindGroup = bind_group;

    this->desc = std::move(desc);
}

WGBindingGroup::~WGBindingGroup()
{
    wgpuBindGroupRelease(bindGroup);
}
