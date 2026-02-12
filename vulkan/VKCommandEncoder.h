#ifndef VKCommandEncoder_H_
#define VKCommandEncoder_H_

#include "VKDevice.h"
#include "VKSurface.h"
#include "VKPipeline.h"
#include "VKTexture.h"
#include "VKBuffer.h"

namespace arhi {

class VKBindingGroup : public BindingGroup {
public:
    VkDescriptorSet descriptorSet;
    VkPipelineLayout pipelineLayout;
    void init(VKDevice* device, const BindingGroupDesc* desc);
    virtual ~VKBindingGroup();
};

struct VKFrameBuffer : public FrameBuffer {
    VKDevice *device;
    RenderPassDesc desc;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    FrameState* frameState;

    bool init(VKDevice *device, const RenderPassDesc& desc);
    ~VKFrameBuffer();
};

class VKCommandEncoder : public CommandEncoder {
    bool isBegined = false;

    VKDevice *device;
    VkFence queue_submit_fence = VK_NULL_HANDLE;
    
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    FrameState* frameState = nullptr;
    //std::vector<VKFrameBuffer*> framebuffers;

    VKPipeline* curPipeline = nullptr;

public:
    ~VKCommandEncoder();
    bool beginPass(FrameBuffer* frameBuffer) override;
    void endPass() override;
    void init(VKDevice* device, const CommandEncoderDesc* desc);
    void setPipeline(Pipeline* pipeline) override;
    void setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) override;
    void setIndexBuffer(Buffer*, int offset, IndexFormat indexFormat) override;
    void setVertexBuffer(Buffer*, int offset, int binding) override;
    void drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) override;

    void setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
    void submit() override;
    void wait();
    void reset();
private:
    void cleanup();
};

}

#endif