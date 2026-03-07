#include "Resource.h"
#include "GraphicsDevice.h"

using namespace arhi;


/**
 * Get the number of bytes per pixel for a given texture format
 * @param format Texture format to get byte per pixel for
 * @return Number of bytes per pixel
 */
int arhi::getBytePerPixel(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::R8Unorm:
    case TextureFormat::R8Snorm:
    case TextureFormat::R8Uint:
    case TextureFormat::R8Sint:
        return 1;
    case TextureFormat::R16Unorm:
    case TextureFormat::R16Snorm:
    case TextureFormat::R16Uint:
    case TextureFormat::R16Sint:
    case TextureFormat::R16Float:
    case TextureFormat::RG8Unorm:
    case TextureFormat::RG8Snorm:
    case TextureFormat::RG8Uint:
    case TextureFormat::RG8Sint:
        return 2;
    case TextureFormat::R32Float:
    case TextureFormat::R32Uint:
    case TextureFormat::R32Sint:
    case TextureFormat::RG16Unorm:
    case TextureFormat::RG16Snorm:
    case TextureFormat::RG16Uint:
    case TextureFormat::RG16Sint:
    case TextureFormat::RG16Float:
    case TextureFormat::RGB10A2Uint:
    case TextureFormat::RGB10A2Unorm:
    case TextureFormat::RG11B10Ufloat:
    case TextureFormat::RGB9E5Ufloat:
        return 4;
    case TextureFormat::RG32Float:
    case TextureFormat::RG32Uint:
    case TextureFormat::RG32Sint:
        return 8;
    case TextureFormat::RGBA8Unorm:
    case TextureFormat::RGBA8UnormSrgb:
    case TextureFormat::RGBA8Snorm:
    case TextureFormat::RGBA8Uint:
    case TextureFormat::RGBA8Sint:
    case TextureFormat::BGRA8Unorm:
    case TextureFormat::BGRA8UnormSrgb:
        return 4;
    case TextureFormat::RGBA16Unorm:
    case TextureFormat::RGBA16Snorm:
    case TextureFormat::RGBA16Uint:
    case TextureFormat::RGBA16Sint:
    case TextureFormat::RGBA16Float:
        return 8;
    case TextureFormat::RGBA32Float:
    case TextureFormat::RGBA32Uint:
    case TextureFormat::RGBA32Sint:
        return 16;
    case TextureFormat::Stencil8:
        return 1;
    case TextureFormat::Depth16Unorm:
        return 2;
    case TextureFormat::Depth24Plus:
        return 3;
    case TextureFormat::Depth24PlusStencil8:
        return 4;
    case TextureFormat::Depth32Float:
        return 4;
    case TextureFormat::Depth32FloatStencil8:
        return 5;
    default:
        ARHI_ERROR("ERROR: unsupport texture format %d\n", format);
        return 4; // Default to 4 bytes per pixel
    }
}