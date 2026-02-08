#ifndef GLCommandEncoder_H_
#define GLCommandEncoder_H_

#include "GLDevice.h"
#include "GLSurface.h"
#include "GLPipeline.h"
#include "GLTexture.h"
#include "GLBuffer.h"

namespace mrhi {

class GLBindingGroup : public BindingGroup {
public:
    BindingGroupDesc desc;

    void init(GLDevice* device, const BindingGroupDesc* desc);
    virtual ~GLBindingGroup();
    void bind();
};

struct GLFrameBuffer : public FrameBuffer {
    //bool isSurface;
    GLuint draftFrameBuffer = 0;
    GLuint frameBuffer;
    GLuint depthStencilBuffer;
    RenderPassDesc desc;

    bool update(GLDevice* device, const RenderPassDesc& desc);
    void resetDraftFrameBuffer();
    ~GLFrameBuffer();
};

class GLCommandEncoder : public CommandEncoder {
public:
    GLDevice *device;
    

    GLPipeline* curPipeline = nullptr;
    GLBuffer* curIndexBuffer = nullptr;
    GLBuffer* curVertexBuffer = nullptr;

    GLuint savedFrameBuffer = 0;
    GLFrameBuffer* frameBuffer = nullptr;
    IndexFormat indexFormat = IndexFormat::Undefined;
public:
    ~GLCommandEncoder();
    bool beginPass(FrameBuffer* frameBuffer) override;
    void endPass() override;
    void init(GLDevice* device, const CommandEncoderDesc* desc);
    void setPipeline(Pipeline* pipeline) override;
    void setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) override;
    void setIndexBuffer(Buffer*, int offset, IndexFormat indexFormat) override;
    void setVertexBuffer(Buffer*, int offset, int binding) override;
    void drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) override;

    void setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
    void submit() override;
    
private:
    void bind();
    void unbind();
};

}

#endif