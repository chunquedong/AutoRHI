#include "GLBuffer.h"

using namespace arhi;

void GLBuffer::init(GLDevice* device, const BufferDesc* desc) {
    this->device = device;
    GLuint vbo;
    GL_ASSERT(glGenBuffers(1, &vbo));
    this->buffer = vbo;
    this->desc = *desc;
}
void GLBuffer::setData(int offset, const void* data, int len) {
    GLuint vbo = (GLuint)buffer;
    int gltype = GL_ARRAY_BUFFER;
    if ((desc.usage & BufferDesc::Usage_Index) != 0) {
        gltype = GL_ELEMENT_ARRAY_BUFFER;
    }
    else if ((desc.usage & BufferDesc::Usage_Uniform) != 0) {
        gltype = GL_UNIFORM_BUFFER;
    }
    else if ((desc.usage & BufferDesc::Usage_Storage) != 0) {
        gltype = GL_SHADER_STORAGE_BUFFER;
    }

    GL_ASSERT(glBindBuffer(gltype, vbo));

    if (offset) {
        GL_ASSERT(glBufferSubData(gltype, offset, len, data));
    }
    else {
        GL_ASSERT(glBufferData(gltype, len, data
            , (desc.usage & BufferDesc::Usage_MapWrite) != 0 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    }

    GL_ASSERT(glBindBuffer(gltype, 0));
}

GLBuffer::~GLBuffer() {
    if (buffer && GraphicsDevice::cur()) {
        GLuint vbo = (GLuint)buffer;
        glDeleteBuffers(1, &vbo);
    }
}