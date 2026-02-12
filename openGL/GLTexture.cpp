#include "GLTexture.h"

using namespace arhi;

GLint getFormatInternal(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::Undefined:
        return 0;
        //auto size type
    //case TextureFormat::RGB8Unorm:
    //    return GL_RGB;
    case TextureFormat::RGBA8Unorm:
        return GL_RGBA;
    //case Image::ALPHA:
    //    return GL_ALPHA;
    case TextureFormat::R8Unorm:
        return GL_R8;
    case TextureFormat::RG8Unorm:
        return GL_RG;

        //fix size type
    //case Image::RGB888:
    //    return GL_RGB8;
    //case Image::RGB565:
    //    return GL_RGB565;
    //case Image::RGBA4444:
    //    return GL_RGBA4;
    //case Image::RGBA5551:
    //    return GL_RGB5_A1;
    //case Image::RGBA8888:
    //    return GL_RGBA8;

        //depth
    case TextureFormat::Depth32Float:
        return GL_DEPTH_COMPONENT32F;
    case TextureFormat::Depth24PlusStencil8:
        return GL_DEPTH24_STENCIL8;

        //float type
    /*case Image::RGB16F:
        return GL_RGB16F;*/
    case TextureFormat::RGBA16Float:
        return GL_RGBA16F;
    case TextureFormat::R16Float:
        return GL_R16F;
    case TextureFormat::RG11B10Ufloat:
        return GL_R11F_G11F_B10F;
    case TextureFormat::RGB9E5Ufloat:
        return GL_RGB9_E5;
    case TextureFormat::R32Float:
        return GL_R32F;
    //case TextureFormat::RGB32:
    //    return GL_RGB32F;
    case TextureFormat::RGBA32Float:
        return GL_RGBA32F;
    case TextureFormat::RG16Float:
        return GL_RG16F;
    default:
        MGP_ERROR("ERROR: unsupport texture format %d\n", format);
        return 0;
    }
}

GLenum getIOFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::Undefined:
        return 0;
        //auto size type
    /*case Image::RGB:
        return GL_RGB;*/
    case TextureFormat::RGBA8Unorm:
        return GL_RGBA;
    //case Image::ALPHA:
    //    return GL_ALPHA;
    case TextureFormat::R8Unorm:
        return GL_RED;
    case TextureFormat::RG8Unorm:
        return GL_RG;

        //fix size type
    //case Image::RGB888:
    //    return GL_RGB;
    //case Image::RGB565:
    //    return GL_RGB;
    //case Image::RGBA4444:
    //    return GL_RGBA;
    //case Image::RGBA5551:
    //    return GL_RGBA;
    //case Image::RGBA8888:
    //    return GL_RGBA;

        //depth
    case TextureFormat::Depth32Float:
        return GL_DEPTH_COMPONENT;
    case TextureFormat::Depth24PlusStencil8:
        return GL_DEPTH_STENCIL;

        //float type
    //case Image::RGB16F:
    //    return GL_RGB;
    case TextureFormat::RGBA16Float:
        return GL_RGBA;
    case TextureFormat::R16Float:
        return GL_RED;
    case TextureFormat::RG11B10Ufloat:
        return GL_RGB;
    case TextureFormat::RGB9E5Ufloat:
        return GL_RGB;
    case TextureFormat::R32Float:
        return GL_RED;
    //case Image::RGB32F:
    //    return GL_RGB;
    case TextureFormat::RGBA32Float:
        return GL_RGBA;
    case TextureFormat::RG16Float:
        return GL_RG;
    default:
        return 0;
    }
}

GLenum getFormatDataType(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::Undefined:
        return 0;
        //auto size type
    //case Image::RGB:
    //    return GL_UNSIGNED_BYTE;
    case TextureFormat::RGBA8Unorm:
        return GL_UNSIGNED_BYTE;
    //case Image::ALPHA:
    //    return GL_UNSIGNED_BYTE;
    case TextureFormat::R8Unorm:
        return GL_UNSIGNED_BYTE;
    case TextureFormat::RG8Unorm:
        return GL_UNSIGNED_BYTE;

        //fix size type
    //case Image::RGB888:
    //    return GL_UNSIGNED_BYTE;
    //case Image::RGB565:
    //    return GL_UNSIGNED_SHORT_5_6_5;
    //case Image::RGBA4444:
    //    return GL_UNSIGNED_SHORT_4_4_4_4;
    //case Image::RGBA5551:
    //    return GL_UNSIGNED_SHORT_5_5_5_1;
    //case Image::RGBA8888:
    //    return GL_UNSIGNED_BYTE;

        //depth
    case TextureFormat::Depth32Float:
        return GL_FLOAT;
    case TextureFormat::Depth24PlusStencil8:
        return GL_UNSIGNED_INT_24_8;

        //float type
    //case Image::RGB16F:
    //    return GL_FLOAT;
    case TextureFormat::RGBA16Float:
        return GL_FLOAT;
    case TextureFormat::R16Float:
        return GL_FLOAT;
    case TextureFormat::RG11B10Ufloat:
        return GL_FLOAT;
    case TextureFormat::RGB9E5Ufloat:
        return GL_FLOAT;
    case TextureFormat::R32Float:
        return GL_FLOAT;
    //case Image::RGB32F:
    //    return GL_FLOAT;
    case TextureFormat::RGBA32Float:
        return GL_FLOAT;
    case TextureFormat::RG16Float:
        return GL_FLOAT;
    default:
        return 0;
    }
}

void GLTexture::init(GLDevice* adevice, const TextureDesc* desc) {
    this->desc = *desc;

    GLenum target = GL_TEXTURE_2D;
    switch (desc->type) {
    case TextureType::_1D:
        target = GL_TEXTURE_1D;
        break;
    case TextureType::_2D:
        target = GL_TEXTURE_2D;
        break;
    case TextureType::Cube:
        target = GL_TEXTURE_CUBE_MAP;
        break;
    case TextureType::_3D:
        target = GL_TEXTURE_3D;
        break;
    case TextureType::_2DArray:
        target = GL_TEXTURE_2D_ARRAY;
        break;
    }

    // Create the texture.
    GLuint textureId;
    GL_ASSERT(glGenTextures(1, &textureId));
    this->texture = textureId;
    GL_ASSERT(glBindTexture(target, textureId));
    GL_ASSERT(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
#ifndef OPENGL_ES
    // glGenerateMipmap is new in OpenGL 3.0. For OpenGL 2.0 we must fallback to use glTexParameteri
    // with GL_GENERATE_MIPMAP prior to actual texture creation (glTexImage2D)
    if (desc->mipLevelCount > 1 && !std::addressof(glGenerateMipmap))
        GL_ASSERT(glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE));
#endif

    GL_ASSERT(glBindTexture(target, 0));

    this->device = adevice;
    this->texture = texture;
    this->targetType = target;
}

void GLTexture::setData(const void* textureData, int mipLevel, int depthOrArrayLayers) {
    if (!texture) {
        MGP_ERROR("Null Texture\n");
        abort();
        return;
    }

    unsigned int width = desc.width;
    unsigned int height = desc.height;

    GLenum target = this->targetType;

    GLint internalFormat = getFormatInternal(desc.format);
    assert(internalFormat != 0);

    GLenum texelType = getFormatDataType(desc.format);
    assert(texelType != 0);

    GLenum ioFormat = getIOFormat(desc.format);
    assert(ioFormat != 0);

    GLuint textureId = this->texture;
    GL_ASSERT(glBindTexture(target, textureId));

    // Load the texture
    size_t bpp = desc.bytePerPixel;
    if (desc.type == TextureType::_2D)
    {
        GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, ioFormat, texelType, textureData));
    }
    else if (desc.type == TextureType::_2DArray) {
        //(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
        GL_ASSERT(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, 0, 0, ioFormat, texelType, NULL));

        const unsigned char* texturePtr = (const unsigned char*)textureData;
        //(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
        GL_ASSERT(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, depthOrArrayLayers, width, height, 1, ioFormat, texelType, texturePtr));
    }
    else if (desc.type == TextureType::Cube)
    {
        // Texture Cube
        const unsigned char* texturePtr = (const unsigned char*)textureData;
        GL_ASSERT(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + mipLevel, 0, internalFormat, width, height, 0, ioFormat, texelType, texturePtr));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else {
        MGP_ERROR("Unsupport Texture type: %d\n", desc.type);
    }

    GL_ASSERT(glBindTexture(target, 0));
}

GLTexture::~GLTexture() {
    if (texture && GraphicsDevice::cur())
    {
        GL_ASSERT(glDeleteTextures(1, &texture));
        texture = 0;
    }
}

GLSampler::~GLSampler()
{
    if (sampler && GraphicsDevice::cur())
    {
        GL_ASSERT(glDeleteSamplers(1, &sampler));
        sampler = 0;
    }
}

GLenum filterToGl(SamplerFilterMode filter) {
    switch (filter)
    {
    case SamplerFilterMode::Undefined:
        break;
    case SamplerFilterMode::Nearest:
        return GL_NEAREST;
        break;
    case SamplerFilterMode::Linear:
        return GL_LINEAR;
        break;
    default:
        break;
    }
    return 0;
}

GLenum wrapModeToGl(SamplerAddressMode mode) {
    switch (mode)
    {
    case SamplerAddressMode::Undefined:
        break;
    case SamplerAddressMode::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
        break;
    case SamplerAddressMode::Repeat:
        return GL_REPEAT;
        break;
    case SamplerAddressMode::MirrorRepeat:
        return GL_MIRRORED_REPEAT;
        break;
    default:
        break;
    }
    return 0;
}

void GLSampler::init(GLDevice* device, const SamplerDesc* desc)
{
    glGenSamplers(1, &sampler);

    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, filterToGl(desc->minFilter)));
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, filterToGl(desc->magFilter)));
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapModeToGl(desc->addressModeU)));
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapModeToGl(desc->addressModeV)));
#if defined(GL_TEXTURE_WRAP_R) // OpenGL ES 3.x and up, OpenGL 1.2 and up
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wrapModeToGl(desc->addressModeW)));
#endif

    if (desc->maxAnisotropy > 1) {
        GLfloat max_tex = 1;
        GL_ASSERT(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_tex));
        //printf("GL_TEXTURE_MAX_ANISOTROPY_EXT:%d\n", max_tex);
        GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, (int)max_tex));
    }
}

