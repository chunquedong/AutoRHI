#include "GLCommandEncoder.h"
#include "GLBuffer.h"
#include "GLPipeline.h"
#include "GLTexture.h"

using namespace arhi;

void GLCommandEncoder::init(GLDevice* device, const CommandEncoderDesc* desc) {
    this->device = device;
}

GLCommandEncoder::~GLCommandEncoder() {
}


bool GLCommandEncoder::beginPass(FrameBuffer* aframeBuffer) {
    this->frameBuffer = dynamic_cast<GLFrameBuffer*>(aframeBuffer);

    const RenderPassDesc& desc = frameBuffer->desc;

    //store old frameBuffer
    GLint fbo = 0;
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo));
    savedFrameBuffer = fbo;

    if (savedFrameBuffer != frameBuffer->frameBuffer) {
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)frameBuffer->frameBuffer));
    }

    GLbitfield clearBits = 0;
    for (int i = 0; i < desc.colorAttachments.size(); ++i) {
        if (desc.colorAttachments[i].loadOp == LoadOp::Clear) {
            clearBits |= GL_COLOR_BUFFER_BIT;
            const float* clearColor = desc.colorAttachments[i].clearValue;
            GL_ASSERT(glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]));
        }
    }

    if (desc.depthStencilAttachment) {
        if (desc.depthStencilAttachment->depthLoadOp == LoadOp::Clear) {
            clearBits |= GL_DEPTH_BUFFER_BIT;
            GL_ASSERT(glClearDepth(desc.depthStencilAttachment->depthClearValue));
        }
        if (desc.depthStencilAttachment->stencilLoadOp == LoadOp::Clear) {
            clearBits |= GL_STENCIL_BUFFER_BIT;
            GL_ASSERT(glClearStencil(desc.depthStencilAttachment->stencilClearValue));
        }
    }

    if (desc.colorAttachments[0].loadOp == LoadOp::Clear) {
        GL_ASSERT(glClear(clearBits));
    }

    return true;
}

void GLCommandEncoder::endPass() {
    unbind();
    curPipeline = nullptr;
    curIndexBuffer = nullptr;
    curVertexBuffer = nullptr;

    //frameBuffer->resetDraftFrameBuffer();
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, savedFrameBuffer));
}

void GLCommandEncoder::submit() {
    glFlush();
}

void GLCommandEncoder::setPipeline(Pipeline* pipeline) {
    GLPipeline* wgPipeline = dynamic_cast<GLPipeline*>(pipeline);
    curPipeline = wgPipeline;

    GL_ASSERT(glUseProgram(wgPipeline->program));
    
    wgPipeline->applyState();
}

void GLCommandEncoder::setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) {
    GLBindingGroup* wgBindingGroup = dynamic_cast<GLBindingGroup*>(bindingGroup);
    wgBindingGroup->bind(curPipeline);
}

void GLCommandEncoder::setIndexBuffer(Buffer* b, int offset, IndexFormat indexFormat) {
    GLBuffer* t = dynamic_cast<GLBuffer*>(b);
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->buffer));
    curIndexBuffer = t;
    this->indexFormat = indexFormat;
}

void GLCommandEncoder::setVertexBuffer(Buffer* b, int offset, int binding) {
    GLBuffer* t = dynamic_cast<GLBuffer*>(b);
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, t->buffer));
    curVertexBuffer = t;
}

void GLCommandEncoder::drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) {
    bind();

    GLenum primitiveType = GL_LINES;
    switch (curPipeline->desc.primitive.topology) {
    case PrimitiveTopology::LineList:
        primitiveType = GL_LINES;
        break;
    case PrimitiveTopology::LineStrip:
        primitiveType = GL_LINE_STRIP;
        break;
    case PrimitiveTopology::PointList:
        primitiveType = GL_POINTS;
        break;
    case PrimitiveTopology::TriangleList:
        primitiveType = GL_TRIANGLES;
        break;
    case PrimitiveTopology::TriangleStrip:
        primitiveType = GL_TRIANGLE_STRIP;
        break;
    }

    GLenum indexFormat = GL_UNSIGNED_INT;
    if (this->indexFormat == IndexFormat::Uint16) {
        indexFormat = GL_UNSIGNED_SHORT;
    }

    if (instances > 1) {
        if (baseVertex == 0 && firstinstance == 0) {
            GL_ASSERT(glDrawElementsInstanced(
                primitiveType,       // mode
                indices,             // count
                indexFormat,         // type
                (GLvoid*)firstindex, // indices
                instances           // instanceCount
            ));
        }
        else {
#ifdef GLAD
            ARHI_ERROR("Unspport glDrawElementsInstancedBaseVertexBaseInstance!");
#else
            // Use instanced rendering
            GL_ASSERT(glDrawElementsInstancedBaseVertexBaseInstance(
                primitiveType,       // mode
                indices,             // count
                indexFormat,         // type
                (GLvoid*)firstindex, // indices
                instances,           // instanceCount
                baseVertex,          // baseVertex
                firstinstance        // baseInstance
            ));
#endif
        }
    } else {
        // Use regular rendering
        if (baseVertex != 0 || firstinstance != 0) {
            // Use base vertex if needed
            GL_ASSERT(glDrawElementsBaseVertex(
                primitiveType,       // mode
                indices,             // count
                indexFormat,         // type
                (GLvoid*)firstindex, // indices
                baseVertex           // baseVertex
            ));
        } else {
            // Simple case
            GL_ASSERT(glDrawElements(
                primitiveType,       // mode
                indices,             // count
                indexFormat,         // type
                (GLvoid*)firstindex  // indices
            ));
        }
    }
}

void GLCommandEncoder::draw(uint32_t vertices, uint32_t instances, uint32_t firstvertex, uint32_t firstinstance) {
    bind();

    GLenum primitiveType = GL_LINES;
    switch (curPipeline->desc.primitive.topology) {
    case PrimitiveTopology::LineList:
        primitiveType = GL_LINES;
        break;
    case PrimitiveTopology::LineStrip:
        primitiveType = GL_LINE_STRIP;
        break;
    case PrimitiveTopology::PointList:
        primitiveType = GL_POINTS;
        break;
    case PrimitiveTopology::TriangleList:
        primitiveType = GL_TRIANGLES;
        break;
    case PrimitiveTopology::TriangleStrip:
        primitiveType = GL_TRIANGLE_STRIP;
        break;
    }

    if (instances > 1) {
        if (firstinstance == 0) {
            // Use simple instanced rendering
            GL_ASSERT(glDrawArraysInstanced(
                primitiveType,       // mode
                firstvertex,         // first
                vertices,            // count
                instances            // instanceCount
            ));
        } else {
#ifdef GLAD
            ARHI_ERROR("Unsupport glDrawArraysInstancedBaseInstance!");
#else
            // Use instanced rendering with base instance
            GL_ASSERT(glDrawArraysInstancedBaseInstance(
                primitiveType,       // mode
                firstvertex,         // first
                vertices,            // count
                instances,           // instanceCount
                firstinstance        // baseInstance
            ));
#endif
        }
    } else {
        // Use regular non-instanced rendering
        GL_ASSERT(glDrawArrays(
            primitiveType,       // mode
            firstvertex,         // first
            vertices             // count
        ));
    }
}

void GLCommandEncoder::setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glScissor(x, y, width, height);
}

void GLCommandEncoder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
    glViewport(x, y, width, height);
    glDepthRange(minDepth, maxDepth);
}

void GLCommandEncoder::bind()
{
    assert(curPipeline);

    for (int i = 0; i < curPipeline->desc.bufferLayout.size(); ++i) {
        BufferLayout& layout = curPipeline->desc.bufferLayout[i];
        int stride = layout.stride;
        for (AttributeElement& attribute : layout.elements) {

            if (attribute._location == -1 || attribute._format == -1 || attribute._size == -1) continue;

            GL_ASSERT(glEnableVertexAttribArray(attribute._location));

            void* pointer = NULL;
            switch (attribute._format) {
            case GL_INT:
                //(GLuint index, GLint size, GLenum type, GLsizei stride, const void*pointer)
                GL_ASSERT(glVertexAttribIPointer(attribute._location, (GLint)attribute._size, attribute._format, (GLsizei)stride, pointer));
                break;
            case GL_FLOAT_VEC2:
                //(    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)2, GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer));
                break;
            case GL_FLOAT_VEC3:
                //(    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)3, GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer));
                break;
            case GL_FLOAT_VEC4:
                //(    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)4, GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer));
                break;
            default:
                //(    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)attribute._size, attribute._format, GL_FALSE, (GLsizei)stride, pointer));
                break;
            }

            // Set vertex attribute divisor for instanced rendering
            // If the buffer layout step mode is Instance, set divisor to 1, otherwise 0
            if (layout.stepMode == StepMode::Instance) {
                GL_ASSERT(glVertexAttribDivisor(attribute._location, 1));
            } else {
                GL_ASSERT(glVertexAttribDivisor(attribute._location, 0));
            }
        }
    }
}

void GLCommandEncoder::unbind()
{
    GL_ASSERT(glBindVertexArray(0));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
    for (int i = 0; i < curPipeline->desc.bufferLayout.size(); ++i) {
        BufferLayout& layout = curPipeline->desc.bufferLayout[i];
        for (AttributeElement& attribute : layout.elements) {
            if (attribute._location == -1) continue;
            GL_ASSERT(glDisableVertexAttribArray(attribute._location));
        }
    }

    if (curIndexBuffer) {
        GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    GL_ASSERT(glUseProgram(0));
}


//////////////////////////////////////////////////////////////////////////////////////////


void GLBindingGroup::init(GLDevice* device, BindingGroupDesc&& desc)
{
    this->desc = std::move(desc);
}

GLBindingGroup::~GLBindingGroup()
{
}

void GLBindingGroup::bind(GLPipeline* curPipeline)
{
    GLPipeline* pipeline = curPipeline;
    //GLPipeline* pipeline = dynamic_cast<GLPipeline*>(desc.pipeline.get());

    for (auto it = desc.resources.begin(); it != desc.resources.end(); ++it) {
        BindingEntry& entry = *it;
        auto found = pipeline->reflection.uniforms.find(entry.name);
        if (found == pipeline->reflection.uniforms.end()) {
            //if (!dynamic_cast<GLTexture*>(entry.resource.get())) {
            //    printf("WARN: Unknow uniform %s\n", entry.name.c_str());
            //}
            continue;
        }
        if (GLTexture* tex = dynamic_cast<GLTexture*>(entry.resource.get())) {
            //GP_ASSERT((sampler->getType() == Texture::TEXTURE_2D && uniform->_type == GL_SAMPLER_2D) ||
            //    (sampler->getType() == Texture::TEXTURE_CUBE && uniform->_type == GL_SAMPLER_CUBE));
            int unit = found->second.binding + entry.offset;
            int location = found->second.glLocation;
            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + unit));
            // Bind the sampler - this binds the texture and applies sampler state
            GL_ASSERT(glBindTexture(tex->targetType, tex->texture));
            GL_ASSERT(glUniform1i(location, unit));
        }
        else if (GLSampler* tex = dynamic_cast<GLSampler*>(entry.resource.get())) {
            int unit = found->second.binding + entry.offset;
            int location = found->second.glLocation;

            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + unit));
            if (tex->texture) {
                GLTexture* texture = dynamic_cast<GLTexture*>(tex->texture.get());
                // Bind the sampler - this binds the texture and applies sampler state
                GL_ASSERT(glBindTexture(texture->targetType, texture->texture));
            }

            GL_ASSERT(glBindSampler(unit, tex->sampler));
            GL_ASSERT(glUniform1i(location, unit));
        }
        else if (GLBuffer* buffer = dynamic_cast<GLBuffer*>(entry.resource.get())) {
            
            GL_ASSERT(glBindBufferBase(GL_UNIFORM_BUFFER, found->second.binding, buffer->buffer));
        }
        else {
            ARHI_ERROR("ERROR unknow binding resource type\n");
        }
    }
}

bool GLFrameBuffer::update(GLDevice* device, RenderPassDesc&& desc)
{
    //try bind surface
    GLSurface* surface = nullptr;
    for (int i = 0; i < desc.colorAttachments.size(); ++i) {
        GLTexture* textureView = dynamic_cast<GLTexture*>(desc.colorAttachments[i].view.get());
        if (textureView->fromSurface) {
            surface = textureView->fromSurface;
            break;
        }
    }
    if (surface) {
        if (desc.colorAttachments.size() != 1 || desc.depthStencilAttachment != nullptr) {
            ARHI_ERROR("Surface Unsupport MulitTarget Render\n");
            return false;
        }

        this->frameBuffer = surface->frameBufferId;
        //this->isSurface = true;
    }
    else {
        //init draft fbo
        if (this->draftFrameBuffer == 0) {
            GLuint handle = 0;
            GL_ASSERT(glGenFramebuffers(1, &handle));
            this->draftFrameBuffer = handle;
        }
        this->frameBuffer = this->draftFrameBuffer;

        GLint defaultFbo = 0;
        GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFbo));
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));

        for (int i = 0; i < desc.colorAttachments.size(); ++i) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.colorAttachments[i].view.get());
            GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureView->targetType, textureView->texture, 0));
        }

        if (desc.depthStencilAttachment) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.depthStencilAttachment->view.get());

            GLenum attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            if (textureView->desc.format == TextureFormat::Depth24PlusStencil8)
            {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            else if (textureView->desc.format == TextureFormat::Stencil8)
            {
                attachment = GL_STENCIL_ATTACHMENT;
            }
            else {
                attachment = GL_DEPTH_ATTACHMENT;
            }
            //GL_ASSERT(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL, textureView->desc.width, textureView->desc.height));
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureView->targetType, textureView->texture, 0));
        }

        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, defaultFbo));
            ARHI_ERROR("Framebuffer status incomplete: 0x%x\n", fboStatus);
            return false;
        }

        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, defaultFbo));
    }

    //    if (desc.colorAttachments.size() == 0 && desc.depthStencilAttachment == nullptr) {
    //#if !defined(OPENGL_ES) && !defined(__EMSCRIPTEN__)
    //        glDrawBuffer(GL_NONE);
    //        glReadBuffer(GL_NONE);
    //#elif defined(GL_ES_VERSION_3_0) && GL_ES_VERSION_3_0
    //        glDrawBuffers(0, NULL);
    //#endif
    //    }
    this->desc = std::move(desc);

    return true;
}

void GLFrameBuffer::resetDraftFrameBuffer()
{
    if (draftFrameBuffer && draftFrameBuffer == frameBuffer) {
        for (int i = 0; i < desc.colorAttachments.size(); ++i) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.colorAttachments[i].view.get());
            GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureView->targetType, 0, 0));
        }

        if (desc.depthStencilAttachment) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.depthStencilAttachment->view.get());

            GLenum attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            if (textureView->desc.format == TextureFormat::Depth24PlusStencil8)
            {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            else if (textureView->desc.format == TextureFormat::Stencil8)
            {
                attachment = GL_STENCIL_ATTACHMENT;
            }
            else {
                attachment = GL_DEPTH_ATTACHMENT;
            }
            //GL_ASSERT(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL, textureView->desc.width, textureView->desc.height));
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureView->targetType, 0, 0));
        }
    }
}

GLFrameBuffer::~GLFrameBuffer()
{
    if (GraphicsDevice::cur()) {
        if (draftFrameBuffer) {
            GLuint handle = (GLuint)draftFrameBuffer;
            GL_ASSERT(glDeleteFramebuffers(1, &handle));
            draftFrameBuffer = 0;
        }
        if (depthStencilBuffer)
            GL_ASSERT(glDeleteRenderbuffers(1, &depthStencilBuffer));
    }
}
