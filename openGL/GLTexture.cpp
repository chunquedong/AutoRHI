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
        ARHI_ERROR("ERROR: unsupport texture format %d\n", format);
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
#ifndef GLAD
    case TextureType::_1D:
        target = GL_TEXTURE_1D;
        break;
#endif
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
    GL_ASSERT(glBindTexture(target, 0));

    this->device = adevice;
    this->texture = texture;
    this->targetType = target;
}

void GLTexture::setData(const void* textureData, int mipLevel, int depthOrArrayLayers) {
    if (!texture) {
        ARHI_ERROR("Null Texture\n");
        abort();
        return;
    }

    unsigned int width = desc.width >> mipLevel;
    unsigned int height = desc.height >> mipLevel;
    if (width == 0) width = 1;
    if (height == 0) height = 1;

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
    size_t bpp = getBytePerPixel(desc.format);
    if (desc.type == TextureType::_2D)
    {
        // Always allocate texture storage, even if data is null
        GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, mipLevel, internalFormat, width, height, 0, ioFormat, texelType, textureData));
        // Generate mipmaps if this is the base level and mipmap count > 1
        if (mipLevel == 0 && desc.mipLevelCount > 1) {
            GL_ASSERT(glGenerateMipmap(target));
        }
    }
    else if (desc.type == TextureType::_2DArray) {
        // Always allocate texture storage, even if data is null
        //(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
        GL_ASSERT(glTexImage3D(GL_TEXTURE_2D_ARRAY, mipLevel, internalFormat, width, height, depthOrArrayLayers, 0, ioFormat, texelType, textureData));
        // Generate mipmaps if this is the base level and mipmap count > 1
        if (mipLevel == 0 && desc.mipLevelCount > 1) {
            GL_ASSERT(glGenerateMipmap(target));
        }
    }
    else if (desc.type == TextureType::Cube)
    {
        // Texture Cube
        const unsigned char* texturePtr = (const unsigned char*)textureData;
        size_t faceSize = width * height * bpp;
        
        // Set each cube face
        for (int face = 0; face < 6; face++) {
            GLenum faceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
            const void* faceData = textureData ? (texturePtr + (face * faceSize)) : nullptr;
            GL_ASSERT(glTexImage2D(faceTarget, mipLevel, internalFormat, width, height, 0, ioFormat, texelType, faceData));
        }
        // Generate mipmaps if this is the base level and mipmap count > 1
        if (mipLevel == 0 && desc.mipLevelCount > 1) {
            GL_ASSERT(glGenerateMipmap(target));
            // Set min filter with mipmap support for cube maps
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            // Set min filter without mipmap for cube maps
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        // Set mag filter (no mipmap for magnification)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else {
        ARHI_ERROR("Unsupport Texture type: %d\n", desc.type);
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

GLenum getMinFilterWithMipmap(SamplerFilterMode minFilter, SamplerFilterMode mipmapFilter) {
    switch (minFilter) {
    case SamplerFilterMode::Nearest:
        switch (mipmapFilter) {
        case SamplerFilterMode::Nearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case SamplerFilterMode::Linear:
            return GL_NEAREST_MIPMAP_LINEAR;
        default:
            return GL_NEAREST;
        }
    case SamplerFilterMode::Linear:
        switch (mipmapFilter) {
        case SamplerFilterMode::Nearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case SamplerFilterMode::Linear:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_LINEAR;
        }
    default:
        return GL_NEAREST;
    }
}

void GLSampler::init(GLDevice* device, const SamplerDesc* desc)
{
    glGenSamplers(1, &sampler);

    bool isMipmap = false;
    if (texture) {
        GLTexture* tx = dynamic_cast<GLTexture*>(texture.get());
        if (tx && tx->desc.mipLevelCount > 1) {
            isMipmap = true;
        }
    }

    if (isMipmap) {
        // Set min filter with mipmap support
        GLenum minFilter = getMinFilterWithMipmap(desc->minFilter, desc->mipmapFilter);
        GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minFilter));
    }
    else {
        GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, filterToGl(desc->minFilter)));
    }

    // Set mag filter (no mipmap for magnification)
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, filterToGl(desc->magFilter)));
    
    // Set wrap modes
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapModeToGl(desc->addressModeU)));
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapModeToGl(desc->addressModeV)));
#if defined(GL_TEXTURE_WRAP_R) // OpenGL ES 3.x and up, OpenGL 1.2 and up
    GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wrapModeToGl(desc->addressModeW)));
#endif

    // Set LOD range
    GL_ASSERT(glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, desc->lodMinClamp));
    GL_ASSERT(glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, desc->lodMaxClamp));

    // Set anisotropy if requested
    if (desc->maxAnisotropy > 1) {
#ifndef GLAD
        GLfloat max_tex = 1;
        GL_ASSERT(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_tex));
        //printf("GL_TEXTURE_MAX_ANISOTROPY_EXT:%d\n", max_tex);
        GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, (int)max_tex));
#endif
    }

    // Set compare function if needed
    if (desc->compare != CompareFunction::Undefined) {
        GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        switch (desc->compare) {
        case CompareFunction::Never:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_NEVER));
            break;
        case CompareFunction::Less:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_LESS));
            break;
        case CompareFunction::Equal:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_EQUAL));
            break;
        case CompareFunction::LessEqual:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
            break;
        case CompareFunction::Greater:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_GREATER));
            break;
        case CompareFunction::NotEqual:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_NOTEQUAL));
            break;
        case CompareFunction::GreaterEqual:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL));
            break;
        case CompareFunction::Always:
            GL_ASSERT(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_ALWAYS));
            break;
        }
    }
}

