/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef Resource_H_
#define Resource_H_

#include "rhi_common.h"

namespace arhi {

/**
 * Base class for all resources
 */
struct Resource {
    virtual ~Resource() {}
};

/**
 * Texture format enumeration
 * Defines various texture formats with different pixel layouts and data types
 */
enum struct TextureFormat {
    Undefined = 0x00000000,           ///< Undefined texture format
    R8Unorm = 0x00000001,              ///< 8-bit unsigned normalized red channel
    R8Snorm = 0x00000002,              ///< 8-bit signed normalized red channel
    R8Uint = 0x00000003,                ///< 8-bit unsigned integer red channel
    R8Sint = 0x00000004,                ///< 8-bit signed integer red channel
    R16Unorm = 0x00000005,             ///< 16-bit unsigned normalized red channel
    R16Snorm = 0x00000006,             ///< 16-bit signed normalized red channel
    R16Uint = 0x00000007,               ///< 16-bit unsigned integer red channel
    R16Sint = 0x00000008,               ///< 16-bit signed integer red channel
    R16Float = 0x00000009,              ///< 16-bit floating point red channel
    RG8Unorm = 0x0000000A,              ///< 8-bit unsigned normalized red-green channels
    RG8Snorm = 0x0000000B,              ///< 8-bit signed normalized red-green channels
    RG8Uint = 0x0000000C,                ///< 8-bit unsigned integer red-green channels
    RG8Sint = 0x0000000D,                ///< 8-bit signed integer red-green channels
    R32Float = 0x0000000E,              ///< 32-bit floating point red channel
    R32Uint = 0x0000000F,                ///< 32-bit unsigned integer red channel
    R32Sint = 0x00000010,                ///< 32-bit signed integer red channel
    RG16Unorm = 0x00000011,             ///< 16-bit unsigned normalized red-green channels
    RG16Snorm = 0x00000012,             ///< 16-bit signed normalized red-green channels
    RG16Uint = 0x00000013,               ///< 16-bit unsigned integer red-green channels
    RG16Sint = 0x00000014,               ///< 16-bit signed integer red-green channels
    RG16Float = 0x00000015,              ///< 16-bit floating point red-green channels
    RGBA8Unorm = 0x00000016,            ///< 8-bit unsigned normalized RGBA channels
    RGBA8UnormSrgb = 0x00000017,        ///< 8-bit unsigned normalized RGBA channels with sRGB encoding
    RGBA8Snorm = 0x00000018,            ///< 8-bit signed normalized RGBA channels
    RGBA8Uint = 0x00000019,              ///< 8-bit unsigned integer RGBA channels
    RGBA8Sint = 0x0000001A,              ///< 8-bit signed integer RGBA channels
    BGRA8Unorm = 0x0000001B,            ///< 8-bit unsigned normalized BGRA channels
    BGRA8UnormSrgb = 0x0000001C,        ///< 8-bit unsigned normalized BGRA channels with sRGB encoding
    RGB10A2Uint = 0x0000001D,           ///< 10-bit RGB + 2-bit alpha unsigned integer
    RGB10A2Unorm = 0x0000001E,          ///< 10-bit RGB + 2-bit alpha unsigned normalized
    RG11B10Ufloat = 0x0000001F,         ///< 11-bit RG + 10-bit B unsigned floating point
    RGB9E5Ufloat = 0x00000020,          ///< 9-bit RGB with shared 5-bit exponent unsigned floating point
    RG32Float = 0x00000021,             ///< 32-bit floating point red-green channels
    RG32Uint = 0x00000022,               ///< 32-bit unsigned integer red-green channels
    RG32Sint = 0x00000023,               ///< 32-bit signed integer red-green channels
    RGBA16Unorm = 0x00000024,           ///< 16-bit unsigned normalized RGBA channels
    RGBA16Snorm = 0x00000025,           ///< 16-bit signed normalized RGBA channels
    RGBA16Uint = 0x00000026,             ///< 16-bit unsigned integer RGBA channels
    RGBA16Sint = 0x00000027,             ///< 16-bit signed integer RGBA channels
    RGBA16Float = 0x00000028,            ///< 16-bit floating point RGBA channels
    RGBA32Float = 0x00000029,            ///< 32-bit floating point RGBA channels
    RGBA32Uint = 0x0000002A,             ///< 32-bit unsigned integer RGBA channels
    RGBA32Sint = 0x0000002B,             ///< 32-bit signed integer RGBA channels
    Stencil8 = 0x0000002C,               ///< 8-bit stencil channel
    Depth16Unorm = 0x0000002D,           ///< 16-bit unsigned normalized depth
    Depth24Plus = 0x0000002E,            ///< 24-bit depth plus stencil
    Depth24PlusStencil8 = 0x0000002F,    ///< 24-bit depth + 8-bit stencil
    Depth32Float = 0x00000030,           ///< 32-bit floating point depth
    Depth32FloatStencil8 = 0x00000031,   ///< 32-bit floating point depth + 8-bit stencil
    BC1RGBAUnorm = 0x00000032,           ///< BC1 compressed RGBA unsigned normalized
    BC1RGBAUnormSrgb = 0x00000033,       ///< BC1 compressed RGBA unsigned normalized with sRGB encoding
    BC2RGBAUnorm = 0x00000034,           ///< BC2 compressed RGBA unsigned normalized
    BC2RGBAUnormSrgb = 0x00000035,       ///< BC2 compressed RGBA unsigned normalized with sRGB encoding
    BC3RGBAUnorm = 0x00000036,           ///< BC3 compressed RGBA unsigned normalized
    BC3RGBAUnormSrgb = 0x00000037,       ///< BC3 compressed RGBA unsigned normalized with sRGB encoding
    BC4RUnorm = 0x00000038,              ///< BC4 compressed red channel unsigned normalized
    BC4RSnorm = 0x00000039,              ///< BC4 compressed red channel signed normalized
    BC5RGUnorm = 0x0000003A,             ///< BC5 compressed red-green channels unsigned normalized
    BC5RGSnorm = 0x0000003B,             ///< BC5 compressed red-green channels signed normalized
    BC6HRGBUfloat = 0x0000003C,          ///< BC6H compressed RGB unsigned floating point
    BC6HRGBFloat = 0x0000003D,           ///< BC6H compressed RGB floating point
    BC7RGBAUnorm = 0x0000003E,           ///< BC7 compressed RGBA unsigned normalized
    BC7RGBAUnormSrgb = 0x0000003F,       ///< BC7 compressed RGBA unsigned normalized with sRGB encoding
    ETC2RGB8Unorm = 0x00000040,          ///< ETC2 compressed RGB unsigned normalized
    ETC2RGB8UnormSrgb = 0x00000041,      ///< ETC2 compressed RGB unsigned normalized with sRGB encoding
    ETC2RGB8A1Unorm = 0x00000042,        ///< ETC2 compressed RGB with 1-bit alpha unsigned normalized
    ETC2RGB8A1UnormSrgb = 0x00000043,    ///< ETC2 compressed RGB with 1-bit alpha unsigned normalized with sRGB encoding
    ETC2RGBA8Unorm = 0x00000044,         ///< ETC2 compressed RGBA unsigned normalized
    ETC2RGBA8UnormSrgb = 0x00000045,     ///< ETC2 compressed RGBA unsigned normalized with sRGB encoding
    EACR11Unorm = 0x00000046,            ///< EAC compressed 11-bit red channel unsigned normalized
    EACR11Snorm = 0x00000047,            ///< EAC compressed 11-bit red channel signed normalized
    EACRG11Unorm = 0x00000048,           ///< EAC compressed 11-bit red-green channels unsigned normalized
    EACRG11Snorm = 0x00000049,           ///< EAC compressed 11-bit red-green channels signed normalized
    ASTC4x4Unorm = 0x0000004A,           ///< ASTC 4x4 compressed unsigned normalized
    ASTC4x4UnormSrgb = 0x0000004B,       ///< ASTC 4x4 compressed unsigned normalized with sRGB encoding
    ASTC5x4Unorm = 0x0000004C,           ///< ASTC 5x4 compressed unsigned normalized
    ASTC5x4UnormSrgb = 0x0000004D,       ///< ASTC 5x4 compressed unsigned normalized with sRGB encoding
    ASTC5x5Unorm = 0x0000004E,           ///< ASTC 5x5 compressed unsigned normalized
    ASTC5x5UnormSrgb = 0x0000004F,       ///< ASTC 5x5 compressed unsigned normalized with sRGB encoding
    ASTC6x5Unorm = 0x00000050,           ///< ASTC 6x5 compressed unsigned normalized
    ASTC6x5UnormSrgb = 0x00000051,       ///< ASTC 6x5 compressed unsigned normalized with sRGB encoding
    ASTC6x6Unorm = 0x00000052,           ///< ASTC 6x6 compressed unsigned normalized
    ASTC6x6UnormSrgb = 0x00000053,       ///< ASTC 6x6 compressed unsigned normalized with sRGB encoding
    ASTC8x5Unorm = 0x00000054,           ///< ASTC 8x5 compressed unsigned normalized
    ASTC8x5UnormSrgb = 0x00000055,       ///< ASTC 8x5 compressed unsigned normalized with sRGB encoding
    ASTC8x6Unorm = 0x00000056,           ///< ASTC 8x6 compressed unsigned normalized
    ASTC8x6UnormSrgb = 0x00000057,       ///< ASTC 8x6 compressed unsigned normalized with sRGB encoding
    ASTC8x8Unorm = 0x00000058,           ///< ASTC 8x8 compressed unsigned normalized
    ASTC8x8UnormSrgb = 0x00000059,       ///< ASTC 8x8 compressed unsigned normalized with sRGB encoding
    ASTC10x5Unorm = 0x0000005A,          ///< ASTC 10x5 compressed unsigned normalized
    ASTC10x5UnormSrgb = 0x0000005B,      ///< ASTC 10x5 compressed unsigned normalized with sRGB encoding
    ASTC10x6Unorm = 0x0000005C,          ///< ASTC 10x6 compressed unsigned normalized
    ASTC10x6UnormSrgb = 0x0000005D,      ///< ASTC 10x6 compressed unsigned normalized with sRGB encoding
    ASTC10x8Unorm = 0x0000005E,          ///< ASTC 10x8 compressed unsigned normalized
    ASTC10x8UnormSrgb = 0x0000005F,      ///< ASTC 10x8 compressed unsigned normalized with sRGB encoding
    ASTC10x10Unorm = 0x00000060,         ///< ASTC 10x10 compressed unsigned normalized
    ASTC10x10UnormSrgb = 0x00000061,     ///< ASTC 10x10 compressed unsigned normalized with sRGB encoding
    ASTC12x10Unorm = 0x00000062,         ///< ASTC 12x10 compressed unsigned normalized
    ASTC12x10UnormSrgb = 0x00000063,     ///< ASTC 12x10 compressed unsigned normalized with sRGB encoding
    ASTC12x12Unorm = 0x00000064,         ///< ASTC 12x12 compressed unsigned normalized
    ASTC12x12UnormSrgb = 0x00000065,     ///< ASTC 12x12 compressed unsigned normalized with sRGB encoding
    R8BG8Biplanar420Unorm = 0x00050006,  ///< R8BG8 biplanar 4:2:0 unsigned normalized
    R10X6BG10X6Biplanar420Unorm = 0x00050007, ///< R10X6BG10X6 biplanar 4:2:0 unsigned normalized
    R8BG8A8Triplanar420Unorm = 0x00050008, ///< R8BG8A8 triplanar 4:2:0 unsigned normalized
    R8BG8Biplanar422Unorm = 0x00050009,  ///< R8BG8 biplanar 4:2:2 unsigned normalized
    R8BG8Biplanar444Unorm = 0x0005000A,  ///< R8BG8 biplanar 4:4:4 unsigned normalized
    R10X6BG10X6Biplanar422Unorm = 0x0005000B, ///< R10X6BG10X6 biplanar 4:2:2 unsigned normalized
    R10X6BG10X6Biplanar444Unorm = 0x0005000C, ///< R10X6BG10X6 biplanar 4:4:4 unsigned normalized
    External = 0x0005000D,               ///< External texture format
};

/**
 * Texture type enumeration
 * Defines different types of textures with varying dimensions and layouts
 */
enum struct TextureType {
    Undefined = 0x00000000,     ///< Undefined texture type
    _1D = 0x00000001,            ///< 1-dimensional texture
    _2D = 0x00000002,            ///< 2-dimensional texture
    _2DArray = 0x00000003,       ///< 2-dimensional texture array
    Cube = 0x00000004,           ///< Cubemap texture (6 faces)
    CubeArray = 0x00000005,      ///< Cubemap texture array
    _3D = 0x00000006,            ///< 3-dimensional texture
};

/**
 * Calculate bytes per pixel for a given texture format
 * @param format The texture format to calculate for
 * @return Number of bytes per pixel for the given format
 */
int getBytePerPixel(TextureFormat format);

/**
 * Texture descriptor
 * Contains all properties needed to create a texture
 */
struct TextureDesc {
    static const int Usage_None = 0x0000000000000000;             ///< No usage flags
    static const int Usage_CopySrc = 0x0000000000000001;           ///< Can be used as a copy source
    static const int Usage_CopyDst = 0x0000000000000002;           ///< Can be used as a copy destination
    static const int Usage_TextureBinding = 0x0000000000000004;    ///< Can be bound as a texture
    static const int Usage_StorageBinding = 0x0000000000000008;    ///< Can be bound as a storage texture
    static const int Usage_RenderAttachment = 0x0000000000000010;  ///< Can be used as a render attachment
    static const int Usage_TransientAttachment = 0x0000000000001000; ///< Can be used as a transient attachment
    static const int Usage_StorageAttachment = 0x0000000000002000; ///< Can be used as a storage attachment

    int width;                     ///< Texture width in pixels
    int height;                    ///< Texture height in pixels
    TextureFormat format;          ///< Texture format
    TextureType type = TextureType::_2D; ///< Texture type (default: 2D)
    int dimension = 2;             ///< Texture dimension (1, 2, or 3)
    int usage = 0;                 ///< Texture usage flags
    int baseMipLevel = 0;          ///< Base mipmap level
    int mipLevelCount = 1;         ///< Number of mipmap levels
    int baseArrayLayer = 0;        ///< Base array layer
    int arrayLayerCount = 1;       ///< Number of array layers
    uint32_t depthOrArrayLayers = 1; ///< Depth (for 3D textures) or number of array layers
};

/**
 * Sampler address mode enumeration
 * Defines how texture coordinates outside the [0, 1] range are handled
 */
enum struct SamplerAddressMode {
    Undefined = 0x00000000,     ///< Undefined address mode
    ClampToEdge = 0x00000001,    ///< Clamp coordinates to [0, 1]
    Repeat = 0x00000002,         ///< Repeat texture coordinates
    MirrorRepeat = 0x00000003,   ///< Repeat texture coordinates with mirroring
};

/**
 * Sampler filter mode enumeration
 * Defines how texture sampling is performed
 */
enum struct SamplerFilterMode {
    Undefined = 0x00000000,     ///< Undefined filter mode
    Nearest = 0x00000001,       ///< Nearest neighbor filtering
    Linear = 0x00000002,        ///< Linear filtering
};

/**
 * Compare function enumeration
 * Defines comparison operations for depth testing
 */
enum struct CompareFunction {
    Undefined = 0x00000000,     ///< Undefined compare function
    Never = 0x00000001,         ///< Never pass
    Less = 0x00000002,           ///< Pass if source < destination
    Equal = 0x00000003,          ///< Pass if source == destination
    LessEqual = 0x00000004,      ///< Pass if source <= destination
    Greater = 0x00000005,        ///< Pass if source > destination
    NotEqual = 0x00000006,       ///< Pass if source != destination
    GreaterEqual = 0x00000007,   ///< Pass if source >= destination
    Always = 0x00000008,         ///< Always pass
};

/**
 * Sampler descriptor
 * Contains all properties needed to create a sampler
 */
struct SamplerDesc {
    SamplerAddressMode addressModeU = SamplerAddressMode::ClampToEdge; ///< Address mode for U coordinate
    SamplerAddressMode addressModeV = SamplerAddressMode::ClampToEdge; ///< Address mode for V coordinate
    SamplerAddressMode addressModeW = SamplerAddressMode::ClampToEdge; ///< Address mode for W coordinate
    SamplerFilterMode magFilter = SamplerFilterMode::Linear;           ///< Magnification filter
    SamplerFilterMode minFilter = SamplerFilterMode::Nearest;           ///< Minification filter
    SamplerFilterMode mipmapFilter = SamplerFilterMode::Linear;         ///< Mipmap filter
    float lodMinClamp = 0;                                              ///< Minimum LOD clamp
    float lodMaxClamp = 1;                                              ///< Maximum LOD clamp
    CompareFunction compare = CompareFunction::Undefined;               ///< Compare function for depth testing
    uint16_t maxAnisotropy = 1;                                         ///< Maximum anisotropy level
};

/**
 * Texture interface
 * Represents a texture resource that can be bound to shaders or used as render targets
 */
struct Texture : public Resource {
    virtual ~Texture() {}
    
    /**
     * Set texture data
     * @param data Pointer to texture data
     * @param mipLevel Mipmap level to set data for
     * @param depthOrArrayLayers Number of depth slices or array layers to set
     */
    virtual void setData(const void* data, int mipLevel, int depthOrArrayLayers) = 0;
};

/**
 * Sampler interface
 * Represents a texture sampler that defines how textures are sampled
 */
struct Sampler : public Resource {
    APtr<Texture> texture = nullptr; ///< Texture associated with this sampler
    virtual ~Sampler() {}
};

/**
 * Buffer descriptor
 * Contains all properties needed to create a buffer
 */
struct BufferDesc {
    static const int Usage_None = 0x0000000000000000;                     ///< No usage flags
    static const int Usage_MapRead = 0x0000000000000001;                   ///< Can be mapped for reading
    static const int Usage_MapWrite = 0x0000000000000002;                  ///< Can be mapped for writing
    static const int Usage_CopySrc = 0x0000000000000004;                   ///< Can be used as a copy source
    static const int Usage_CopyDst = 0x0000000000000008;                   ///< Can be used as a copy destination
    static const int Usage_Index = 0x0000000000000010;                     ///< Can be used as an index buffer
    static const int Usage_Vertex = 0x0000000000000020;                    ///< Can be used as a vertex buffer
    static const int Usage_Uniform = 0x0000000000000040;                   ///< Can be used as a uniform buffer
    static const int Usage_Storage = 0x0000000000000080;                   ///< Can be used as a storage buffer
    static const int Usage_Indirect = 0x0000000000000100;                  ///< Can be used for indirect drawing
    static const int Usage_QueryResolve = 0x0000000000000200;              ///< Can be used for query resolution
    static const int Usage_ShaderDeviceAddress = 0x0000000010000000;       ///< Can be accessed via shader device address
    static const int Usage_AccelerationStructureInput = 0x0000000020000000; ///< Can be used as acceleration structure input
    static const int Usage_AccelerationStructureStorage = 0x0000000040000000; ///< Can be used as acceleration structure storage
    static const int Usage_ShaderBindingTable = 0x0000000080000000;        ///< Can be used as shader binding table
    static const int Usage_Raytracing = 0x00000000F0000080;                 ///< Can be used for raytracing

    int size;         ///< Buffer size in bytes
    int usage = 0;    ///< Buffer usage flags
};

/**
 * Buffer interface
 * Represents a buffer resource that can store various types of data
 */
struct Buffer : public Resource {
    virtual ~Buffer() {}
    
    /**
     * Set buffer data
     * @param offset Offset in bytes from the start of the buffer
     * @param data Pointer to buffer data
     * @param size Size of data in bytes
     */
    virtual void setData(int offset, const void* data, int size) = 0;
};

}
#endif