#ifndef Resource_H_
#define Resource_H_

#include "rhi_common.h"

namespace mrhi {

struct Resource {
    virtual ~Resource() {}
};

enum struct TextureFormat {
    Undefined = 0x00000000,
    R8Unorm = 0x00000001,
    R8Snorm = 0x00000002,
    R8Uint = 0x00000003,
    R8Sint = 0x00000004,
    R16Unorm = 0x00000005,
    R16Snorm = 0x00000006,
    R16Uint = 0x00000007,
    R16Sint = 0x00000008,
    R16Float = 0x00000009,
    RG8Unorm = 0x0000000A,
    RG8Snorm = 0x0000000B,
    RG8Uint = 0x0000000C,
    RG8Sint = 0x0000000D,
    R32Float = 0x0000000E,
    R32Uint = 0x0000000F,
    R32Sint = 0x00000010,
    RG16Unorm = 0x00000011,
    RG16Snorm = 0x00000012,
    RG16Uint = 0x00000013,
    RG16Sint = 0x00000014,
    RG16Float = 0x00000015,
    RGBA8Unorm = 0x00000016,
    RGBA8UnormSrgb = 0x00000017,
    RGBA8Snorm = 0x00000018,
    RGBA8Uint = 0x00000019,
    RGBA8Sint = 0x0000001A,
    BGRA8Unorm = 0x0000001B,
    BGRA8UnormSrgb = 0x0000001C,
    RGB10A2Uint = 0x0000001D,
    RGB10A2Unorm = 0x0000001E,
    RG11B10Ufloat = 0x0000001F,
    RGB9E5Ufloat = 0x00000020,
    RG32Float = 0x00000021,
    RG32Uint = 0x00000022,
    RG32Sint = 0x00000023,
    RGBA16Unorm = 0x00000024,
    RGBA16Snorm = 0x00000025,
    RGBA16Uint = 0x00000026,
    RGBA16Sint = 0x00000027,
    RGBA16Float = 0x00000028,
    RGBA32Float = 0x00000029,
    RGBA32Uint = 0x0000002A,
    RGBA32Sint = 0x0000002B,
    Stencil8 = 0x0000002C,
    Depth16Unorm = 0x0000002D,
    Depth24Plus = 0x0000002E,
    Depth24PlusStencil8 = 0x0000002F,
    Depth32Float = 0x00000030,
    Depth32FloatStencil8 = 0x00000031,
    BC1RGBAUnorm = 0x00000032,
    BC1RGBAUnormSrgb = 0x00000033,
    BC2RGBAUnorm = 0x00000034,
    BC2RGBAUnormSrgb = 0x00000035,
    BC3RGBAUnorm = 0x00000036,
    BC3RGBAUnormSrgb = 0x00000037,
    BC4RUnorm = 0x00000038,
    BC4RSnorm = 0x00000039,
    BC5RGUnorm = 0x0000003A,
    BC5RGSnorm = 0x0000003B,
    BC6HRGBUfloat = 0x0000003C,
    BC6HRGBFloat = 0x0000003D,
    BC7RGBAUnorm = 0x0000003E,
    BC7RGBAUnormSrgb = 0x0000003F,
    ETC2RGB8Unorm = 0x00000040,
    ETC2RGB8UnormSrgb = 0x00000041,
    ETC2RGB8A1Unorm = 0x00000042,
    ETC2RGB8A1UnormSrgb = 0x00000043,
    ETC2RGBA8Unorm = 0x00000044,
    ETC2RGBA8UnormSrgb = 0x00000045,
    EACR11Unorm = 0x00000046,
    EACR11Snorm = 0x00000047,
    EACRG11Unorm = 0x00000048,
    EACRG11Snorm = 0x00000049,
    ASTC4x4Unorm = 0x0000004A,
    ASTC4x4UnormSrgb = 0x0000004B,
    ASTC5x4Unorm = 0x0000004C,
    ASTC5x4UnormSrgb = 0x0000004D,
    ASTC5x5Unorm = 0x0000004E,
    ASTC5x5UnormSrgb = 0x0000004F,
    ASTC6x5Unorm = 0x00000050,
    ASTC6x5UnormSrgb = 0x00000051,
    ASTC6x6Unorm = 0x00000052,
    ASTC6x6UnormSrgb = 0x00000053,
    ASTC8x5Unorm = 0x00000054,
    ASTC8x5UnormSrgb = 0x00000055,
    ASTC8x6Unorm = 0x00000056,
    ASTC8x6UnormSrgb = 0x00000057,
    ASTC8x8Unorm = 0x00000058,
    ASTC8x8UnormSrgb = 0x00000059,
    ASTC10x5Unorm = 0x0000005A,
    ASTC10x5UnormSrgb = 0x0000005B,
    ASTC10x6Unorm = 0x0000005C,
    ASTC10x6UnormSrgb = 0x0000005D,
    ASTC10x8Unorm = 0x0000005E,
    ASTC10x8UnormSrgb = 0x0000005F,
    ASTC10x10Unorm = 0x00000060,
    ASTC10x10UnormSrgb = 0x00000061,
    ASTC12x10Unorm = 0x00000062,
    ASTC12x10UnormSrgb = 0x00000063,
    ASTC12x12Unorm = 0x00000064,
    ASTC12x12UnormSrgb = 0x00000065,
    R8BG8Biplanar420Unorm = 0x00050006,
    R10X6BG10X6Biplanar420Unorm = 0x00050007,
    R8BG8A8Triplanar420Unorm = 0x00050008,
    R8BG8Biplanar422Unorm = 0x00050009,
    R8BG8Biplanar444Unorm = 0x0005000A,
    R10X6BG10X6Biplanar422Unorm = 0x0005000B,
    R10X6BG10X6Biplanar444Unorm = 0x0005000C,
    External = 0x0005000D,
};

enum struct TextureType {
    Undefined = 0x00000000,
    _1D = 0x00000001,
    _2D = 0x00000002,
    _2DArray = 0x00000003,
    Cube = 0x00000004,
    CubeArray = 0x00000005,
    _3D = 0x00000006,
};

struct TextureDesc {
    static const int Usage_None = 0x0000000000000000;
    static const int Usage_CopySrc = 0x0000000000000001;
    static const int Usage_CopyDst = 0x0000000000000002;
    static const int Usage_TextureBinding = 0x0000000000000004;
    static const int Usage_StorageBinding = 0x0000000000000008;
    static const int Usage_RenderAttachment = 0x0000000000000010;
    static const int Usage_TransientAttachment = 0x0000000000001000;
    static const int Usage_StorageAttachment = 0x0000000000002000;

    int width;
    int height;
    TextureFormat format;
    TextureType type = TextureType::_2D;
    int dimension = 2;
    int usage = 0;
    int baseMipLevel = 0;
    int mipLevelCount = 1;
    int baseArrayLayer = 0;
    int arrayLayerCount = 1;
    int bytePerPixel = 4;
    uint32_t depthOrArrayLayers = 1;
};

enum struct SamplerAddressMode {
    Undefined = 0x00000000,
    ClampToEdge = 0x00000001,
    Repeat = 0x00000002,
    MirrorRepeat = 0x00000003,
};

enum struct SamplerFilterMode {
    Undefined = 0x00000000,
    Nearest = 0x00000001,
    Linear = 0x00000002,
};

enum struct CompareFunction {
    Undefined = 0x00000000,
    Never = 0x00000001,
    Less = 0x00000002,
    Equal = 0x00000003,
    LessEqual = 0x00000004,
    Greater = 0x00000005,
    NotEqual = 0x00000006,
    GreaterEqual = 0x00000007,
    Always = 0x00000008,
};

struct SamplerDesc {
    SamplerAddressMode addressModeU = SamplerAddressMode::ClampToEdge;
    SamplerAddressMode addressModeV = SamplerAddressMode::ClampToEdge;
    SamplerAddressMode addressModeW = SamplerAddressMode::ClampToEdge;
    SamplerFilterMode magFilter = SamplerFilterMode::Linear;
    SamplerFilterMode minFilter = SamplerFilterMode::Nearest;
    SamplerFilterMode mipmapFilter = SamplerFilterMode::Linear;
    float lodMinClamp = 0;
    float lodMaxClamp = 1;
    CompareFunction compare = CompareFunction::Undefined;
    uint16_t maxAnisotropy = 1;
};

struct Texture : public Resource {
    virtual ~Texture() {}
    virtual void setData(const void* data, int mipLevel, int depthOrArrayLayers) = 0;
};

struct Sampler : public Resource {
    Texture* texture = nullptr;
    virtual ~Sampler() {}
};

struct BufferDesc {
    static const int Usage_None = 0x0000000000000000;
    static const int Usage_MapRead = 0x0000000000000001;
    static const int Usage_MapWrite = 0x0000000000000002;
    static const int Usage_CopySrc = 0x0000000000000004;
    static const int Usage_CopyDst = 0x0000000000000008;
    static const int Usage_Index = 0x0000000000000010;
    static const int Usage_Vertex = 0x0000000000000020;
    static const int Usage_Uniform = 0x0000000000000040;
    static const int Usage_Storage = 0x0000000000000080;
    static const int Usage_Indirect = 0x0000000000000100;
    static const int Usage_QueryResolve = 0x0000000000000200;
    static const int Usage_ShaderDeviceAddress = 0x0000000010000000;
    static const int Usage_AccelerationStructureInput = 0x0000000020000000;
    static const int Usage_AccelerationStructureStorage = 0x0000000040000000;
    static const int Usage_ShaderBindingTable = 0x0000000080000000;
    static const int Usage_Raytracing = 0x00000000F0000080;

    int size;
    int usage = 0;
};

struct Buffer : public Resource {
    virtual ~Buffer() {}
    virtual void setData(int offset, const void* data, int size) = 0;
};

}
#endif