#include "GLCommandEncoder.h"
#include "GLBuffer.h"
#include "GLPipeline.h"
#include "GLTexture.h"

using namespace mrhi;

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
}

void GLCommandEncoder::setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) {
    GLBindingGroup* wgBindingGroup = dynamic_cast<GLBindingGroup*>(bindingGroup);
    wgBindingGroup->bind();
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
    GL_ASSERT(glDrawElements(primitiveType, indices, indexFormat, (GLvoid*)firstindex));
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
            switch (attribute._format)
            {
            case GL_INT:
                //(GLuint index, GLint size, GLenum type, GLsizei stride, const void*pointer)
                GL_ASSERT(glVertexAttribIPointer(attribute._location, (GLint)attribute._size, attribute._format, (GLsizei)stride, pointer));
                break;
            case GL_FLOAT_VEC2:
                //(	GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)2, GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer));
                break;
            case GL_FLOAT_VEC3:
                //(	GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)3, GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer));
                break;
            case GL_FLOAT_VEC4:
                //(	GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)4, GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer));
                break;
            default:
                //(	GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
                GL_ASSERT(glVertexAttribPointer(attribute._location, (GLint)attribute._size, attribute._format, GL_FALSE, (GLsizei)stride, pointer));
                break;
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


void GLBindingGroup::init(GLDevice* device, const BindingGroupDesc* desc)
{
    this->desc = *desc;
}

GLBindingGroup::~GLBindingGroup()
{
}

void GLBindingGroup::bind()
{
    GLPipeline* pipeline = dynamic_cast<GLPipeline*>(desc.pipeline);
    for (auto it = desc.resources.begin(); it != desc.resources.end(); ++it) {
        BindingEntry& entry = *it;
        auto found = pipeline->reflection.uniforms.find(entry.name);
        if (found == pipeline->reflection.uniforms.end()) {
            if (!dynamic_cast<GLTexture*>(entry.resource)) {
                printf("WARN: Unknow uniform %s\n", entry.name.c_str());
            }
            continue;
        }
        if (GLTexture* tex = dynamic_cast<GLTexture*>(entry.resource)) {
            //GP_ASSERT((sampler->getType() == Texture::TEXTURE_2D && uniform->_type == GL_SAMPLER_2D) ||
            //    (sampler->getType() == Texture::TEXTURE_CUBE && uniform->_type == GL_SAMPLER_CUBE));
            int unit = found->second.binding;
            int location = found->second.glLocation;
            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + unit));
            // Bind the sampler - this binds the texture and applies sampler state
            GL_ASSERT(glBindTexture(tex->targetType, tex->texture));
            GL_ASSERT(glUniform1i(location, unit));
        }
        else if (GLSampler* tex = dynamic_cast<GLSampler*>(entry.resource)) {
            int unit = found->second.binding;
            int location = found->second.glLocation;

            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + unit));
            if (tex->texture) {
                GLTexture* texture = dynamic_cast<GLTexture*>(tex->texture);
                // Bind the sampler - this binds the texture and applies sampler state
                GL_ASSERT(glBindTexture(texture->targetType, texture->texture));
            }

            GL_ASSERT(glBindSampler(unit, tex->sampler));
            GL_ASSERT(glUniform1i(location, unit));
        }
        else if (GLBuffer* buffer = dynamic_cast<GLBuffer*>(entry.resource)) {
            
            GL_ASSERT(glBindBufferBase(GL_UNIFORM_BUFFER, found->second.binding, buffer->buffer));
        }
        else {
            MGP_ERROR("ERROR unknow binding resource type\n");
        }
    }
}

bool GLFrameBuffer::update(GLDevice* device, const RenderPassDesc& desc)
{
    //try bind surface
    GLSurface* surface = nullptr;
    for (int i = 0; i < desc.colorAttachments.size(); ++i) {
        GLTexture* textureView = dynamic_cast<GLTexture*>(desc.colorAttachments[i].view);
        if (textureView->fromSurface) {
            surface = textureView->fromSurface;
            break;
        }
    }
    if (surface) {
        if (desc.colorAttachments.size() != 1 || desc.depthStencilAttachment != nullptr) {
            MGP_ERROR("Surface Unsupport MulitTarget Render\n");
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
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));

        for (int i = 0; i < desc.colorAttachments.size(); ++i) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.colorAttachments[i].view);
            GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureView->targetType, textureView->texture, 0));
        }

        if (desc.depthStencilAttachment) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.depthStencilAttachment->view);

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
            MGP_ERROR("Framebuffer status incomplete: 0x%x\n", fboStatus);
            return false;
        }
    }

    //    if (desc.colorAttachments.size() == 0 && desc.depthStencilAttachment == nullptr) {
    //#if !defined(OPENGL_ES) && !defined(__EMSCRIPTEN__)
    //        glDrawBuffer(GL_NONE);
    //        glReadBuffer(GL_NONE);
    //#elif defined(GL_ES_VERSION_3_0) && GL_ES_VERSION_3_0
    //        glDrawBuffers(0, NULL);
    //#endif
    //    }
    this->desc = desc;

    return true;
}

void GLFrameBuffer::resetDraftFrameBuffer()
{
    if (draftFrameBuffer && draftFrameBuffer == frameBuffer) {
        for (int i = 0; i < desc.colorAttachments.size(); ++i) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.colorAttachments[i].view);
            GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureView->targetType, 0, 0));
        }

        if (desc.depthStencilAttachment) {
            GLTexture* textureView = dynamic_cast<GLTexture*>(desc.depthStencilAttachment->view);

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
