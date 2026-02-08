#ifndef Pipeline_H_
#define Pipeline_H_

#include "rhi_common.h"

#include "Shader.h"
#include "Resource.h"

namespace mrhi {

struct AttributeElement {
    std::string name;
    int offset;
    int _location = -1;
    int _format = -1;
    //int _stride = -1;
    int _size = -1;
};

enum struct StepMode {
    Undefined = 0x0,
    Vertex = 0x1,
    Instance = 0x2,
};

struct BufferLayout {
    int stride;
    StepMode stepMode = StepMode::Vertex;
    std::vector<AttributeElement> elements;
};

enum struct PrimitiveTopology {
    Undefined = 0x00000000,
    PointList = 0x00000001,
    LineList = 0x00000002,
    LineStrip = 0x00000003,
    TriangleList = 0x00000004,
    TriangleStrip = 0x00000005,
};

enum struct FrontFace {
    Undefined = 0x00000000,
    CCW = 0x00000001,
    CW = 0x00000002,
};

enum struct CullMode {
    Undefined = 0x00000000,
    None = 0x00000001,
    Front = 0x00000002,
    Back = 0x00000003,
};

enum struct BlendOperation {
    Undefined = 0x00000000,
    Add = 0x00000001,
    Subtract = 0x00000002,
    ReverseSubtract = 0x00000003,
    Min = 0x00000004,
    Max = 0x00000005,
};

enum struct BlendFactor {
    Undefined = 0x00000000,
    Zero = 0x00000001,
    One = 0x00000002,
    Src = 0x00000003,
    OneMinusSrc = 0x00000004,
    SrcAlpha = 0x00000005,
    OneMinusSrcAlpha = 0x00000006,
    Dst = 0x00000007,
    OneMinusDst = 0x00000008,
    DstAlpha = 0x00000009,
    OneMinusDstAlpha = 0x0000000A,
    SrcAlphaSaturated = 0x0000000B,
    Constant = 0x0000000C,
    OneMinusConstant = 0x0000000D,
    Src1 = 0x0000000E,
    OneMinusSrc1 = 0x0000000F,
    Src1Alpha = 0x00000010,
    OneMinusSrc1Alpha = 0x00000011,
};

struct BlendComponent {
    BlendOperation operation;
    BlendFactor srcFactor;
    BlendFactor dstFactor;
};

struct BlendState {
    BlendComponent color;
    BlendComponent alpha;
};

struct ColorTargetState {
    static const int WriteMask_None = 0x0000000000000000;
    static const int WriteMask_Red = 0x0000000000000001;
    static const int WriteMask_Green = 0x0000000000000002;
    static const int WriteMask_Blue = 0x0000000000000004;
    static const int WriteMask_Alpha = 0x0000000000000008;
    static const int WriteMask_All = 0x000000000000000F;

    TextureFormat format = TextureFormat::BGRA8Unorm;
    const BlendState* blend = nullptr;
    int writeMask = WriteMask_All;
};

enum struct IndexFormat {
    Undefined = 0x00000000,
    Uint16 = 0x00000001,
    Uint32 = 0x00000002,
};

enum struct PolygonMode {
    Undefined = 0x00000000,
    Fill = 0x00000001,
    Line = 0x00000002,
    Point = 0x00000003,
};

struct PrimitiveState {
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    IndexFormat stripIndexFormat = IndexFormat::Uint32;
    FrontFace frontFace = FrontFace::CCW;
    CullMode cullMode = CullMode::Back;
    bool unclippedDepth = false;
    PolygonMode polygonMode = PolygonMode::Fill;
};

enum struct StencilOperation {
    Undefined = 0x00000000,
    Keep = 0x00000001,
    Zero = 0x00000002,
    Replace = 0x00000003,
    Invert = 0x00000004,
    IncrementClamp = 0x00000005,
    DecrementClamp = 0x00000006,
    IncrementWrap = 0x00000007,
    DecrementWrap = 0x00000008,
};

struct StencilFaceState {
    CompareFunction compare;
    StencilOperation failOp;
    StencilOperation depthFailOp;
    StencilOperation passOp;
};

struct DepthStencilState {
    TextureFormat format;
    bool depthWriteEnabled;
    CompareFunction depthCompare;

    StencilFaceState stencilFront;
    StencilFaceState stencilBack;
    uint32_t stencilReadMask;
    uint32_t stencilWriteMask;
    int32_t depthBias;
    float depthBiasSlopeScale;
    float depthBiasClamp;
};

struct MultisampleState {
    uint32_t count;
    uint32_t mask;
    bool alphaToCoverageEnabled;
};

struct PipelineDesc {
    const char* label = nullptr;
    Shader* vertexShader;
    Shader* fragmentShader;

    std::vector<BufferLayout> bufferLayout;

    PrimitiveState primitive;

    DepthStencilState* depthStencil = nullptr;

    std::vector<ColorTargetState> targets;

    MultisampleState multisample = {
        .count = 1,
        .mask = 0xffffffff,
    };
};

struct Pipeline {
    ShaderReflection reflection;
    virtual ~Pipeline() {}
protected:
    void reflect(const PipelineDesc* desc);
};

enum struct CompositeAlphaMode {
    Auto = 0x00000000,
    Opaque = 0x00000001,
    Premultiplied = 0x00000002,
    Unpremultiplied = 0x00000003,
    Inherit = 0x00000004,
};

struct SurfaceDesc {
    int width;
    int height;
    TextureFormat format = TextureFormat::BGRA8Unorm;
    CompositeAlphaMode alphaMode = CompositeAlphaMode::Opaque;
    void* surfaceChain = nullptr;
};

struct CommandEncoder;
struct FrameBuffer;

struct Surface {
    virtual void resize(int w, int h) = 0;
    virtual ~Surface() {}
    virtual bool nextImage() = 0;
    virtual void present() = 0;

    virtual Texture* getCurTextureView() = 0;
    virtual CommandEncoder* getCurCommandEncoder() = 0;
    virtual FrameBuffer* getCurFrameBuffer() = 0;
    virtual void cacheFrameBuffer(FrameBuffer* fbo) = 0;
};


}

#endif