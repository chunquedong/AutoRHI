/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef Pipeline_H_
#define Pipeline_H_

#include "rhi_common.h"

#include "Shader.h"
#include "Resource.h"

namespace arhi {

/**
 * Attribute element
 * Represents a single attribute element in a buffer layout
 */
struct AttributeElement {
    std::string name;       ///< Name of the attribute
    int offset;             ///< Offset of the attribute in bytes
    int _location = -1;     ///< Location of the attribute
    int _format = -1;       ///< Format of the attribute
    int _size = -1;         ///< Size of the attribute in bytes

    bool operator==(const AttributeElement& other) const {
        return name == other.name &&
               offset == other.offset &&
               _location == other._location &&
               _format == other._format &&
               _size == other._size;
    }

    bool operator<(const AttributeElement& other) const {
        if (name != other.name) return name < other.name;
        if (offset != other.offset) return offset < other.offset;
        if (_location != other._location) return _location < other._location;
        if (_format != other._format) return _format < other._format;
        return _size < other._size;
    }
};

/**
 * Step mode enumeration
 * Defines how vertex attributes are stepped through
 */
enum struct StepMode {
    Undefined = 0x0,     ///< Undefined step mode
    Vertex = 0x1,        ///< Step per vertex
    Instance = 0x2,       ///< Step per instance
};

/**
 * Buffer layout
 * Represents the layout of vertex attributes in a buffer
 */
struct BufferLayout {
    int stride;                      ///< Stride between elements in bytes
    StepMode stepMode = StepMode::Vertex; ///< Step mode for the buffer
    std::vector<AttributeElement> elements; ///< List of attribute elements

    bool operator==(const BufferLayout& other) const {
        return stride == other.stride &&
               stepMode == other.stepMode &&
               elements == other.elements;
    }

    bool operator<(const BufferLayout& other) const {
        if (stride != other.stride) return stride < other.stride;
        if (stepMode != other.stepMode) return static_cast<int>(stepMode) < static_cast<int>(other.stepMode);
        return elements < other.elements;
    }
};

/**
 * Primitive topology enumeration
 * Defines how vertices are interpreted as primitives
 */
enum struct PrimitiveTopology {
    Undefined = 0x00000000,    ///< Undefined topology
    PointList = 0x00000001,     ///< Point list
    LineList = 0x00000002,      ///< Line list
    LineStrip = 0x00000003,      ///< Line strip
    TriangleList = 0x00000004,   ///< Triangle list
    TriangleStrip = 0x00000005,  ///< Triangle strip
};

/**
 * Front face enumeration
 * Defines the front face winding order
 */
enum struct FrontFace {
    Undefined = 0x00000000,    ///< Undefined front face
    CCW = 0x00000001,          ///< Counter-clockwise winding
    CW = 0x00000002,            ///< Clockwise winding
};

/**
 * Cull mode enumeration
 * Defines which faces to cull
 */
enum struct CullMode {
    Undefined = 0x00000000,    ///< Undefined cull mode
    None = 0x00000001,          ///< No culling
    Front = 0x00000002,         ///< Cull front faces
    Back = 0x00000003,          ///< Cull back faces
};

/**
 * Blend operation enumeration
 * Defines how blend factors are combined
 */
enum struct BlendOperation {
    Undefined = 0x00000000,     ///< Undefined blend operation
    Add = 0x00000001,           ///< Add blend factors
    Subtract = 0x00000002,       ///< Subtract blend factors
    ReverseSubtract = 0x00000003, ///< Reverse subtract blend factors
    Min = 0x00000004,            ///< Take minimum of blend factors
    Max = 0x00000005,            ///< Take maximum of blend factors
};

/**
 * Blend factor enumeration
 * Defines the factors used in blending
 */
enum struct BlendFactor {
    Undefined = 0x00000000,         ///< Undefined blend factor
    Zero = 0x00000001,              ///< Zero
    One = 0x00000002,                ///< One
    Src = 0x00000003,                ///< Source color
    OneMinusSrc = 0x00000004,        ///< One minus source color
    SrcAlpha = 0x00000005,           ///< Source alpha
    OneMinusSrcAlpha = 0x00000006,   ///< One minus source alpha
    Dst = 0x00000007,                ///< Destination color
    OneMinusDst = 0x00000008,        ///< One minus destination color
    DstAlpha = 0x00000009,           ///< Destination alpha
    OneMinusDstAlpha = 0x0000000A,   ///< One minus destination alpha
    SrcAlphaSaturated = 0x0000000B,  ///< Source alpha saturated
    Constant = 0x0000000C,            ///< Constant color
    OneMinusConstant = 0x0000000D,    ///< One minus constant color
    Src1 = 0x0000000E,                ///< Source 1 color
    OneMinusSrc1 = 0x0000000F,        ///< One minus source 1 color
    Src1Alpha = 0x00000010,           ///< Source 1 alpha
    OneMinusSrc1Alpha = 0x00000011,   ///< One minus source 1 alpha
};

/**
 * Blend component
 * Represents a single blend component for color or alpha
 */
struct BlendComponent {
    BlendOperation operation;   ///< Blend operation
    BlendFactor srcFactor;      ///< Source blend factor
    BlendFactor dstFactor;      ///< Destination blend factor
};

/**
 * Blend state
 * Represents the blend state for a color target
 */
struct BlendState {
    BlendComponent color;   ///< Color blend component
    BlendComponent alpha;   ///< Alpha blend component

    bool operator==(const BlendState& other) const {
        return color.operation == other.color.operation &&
               color.srcFactor == other.color.srcFactor &&
               color.dstFactor == other.color.dstFactor &&
               alpha.operation == other.alpha.operation &&
               alpha.srcFactor == other.alpha.srcFactor &&
               alpha.dstFactor == other.alpha.dstFactor;
    }

    bool operator!=(const BlendState& other) const {
        return !(*this == other);
    }

    bool operator<(const BlendState& other) const {
        if (color.operation != other.color.operation) return static_cast<int>(color.operation) < static_cast<int>(other.color.operation);
        if (color.srcFactor != other.color.srcFactor) return static_cast<int>(color.srcFactor) < static_cast<int>(other.color.srcFactor);
        if (color.dstFactor != other.color.dstFactor) return static_cast<int>(color.dstFactor) < static_cast<int>(other.color.dstFactor);
        if (alpha.operation != other.alpha.operation) return static_cast<int>(alpha.operation) < static_cast<int>(other.alpha.operation);
        if (alpha.srcFactor != other.alpha.srcFactor) return static_cast<int>(alpha.srcFactor) < static_cast<int>(other.alpha.srcFactor);
        return static_cast<int>(alpha.dstFactor) < static_cast<int>(other.alpha.dstFactor);
    }
};

/**
 * Color target state
 * Represents the state of a color render target
 */
struct ColorTargetState {
    static const int WriteMask_None = 0x0000000000000000;   ///< No write mask
    static const int WriteMask_Red = 0x0000000000000001;    ///< Red channel write mask
    static const int WriteMask_Green = 0x0000000000000002;  ///< Green channel write mask
    static const int WriteMask_Blue = 0x0000000000000004;   ///< Blue channel write mask
    static const int WriteMask_Alpha = 0x0000000000000008;  ///< Alpha channel write mask
    static const int WriteMask_All = 0x000000000000000F;    ///< All channels write mask

    TextureFormat format = TextureFormat::BGRA8Unorm;    ///< Texture format
    bool blendEnabled = false;
    BlendState blend;                  ///< Blend state
    int writeMask = WriteMask_All;                     ///< Write mask

    bool operator==(const ColorTargetState& other) const {
        return format == other.format &&
               blend == other.blend &&
               writeMask == other.writeMask;
    }

    bool operator<(const ColorTargetState& other) const {
        if (format != other.format) return static_cast<int>(format) < static_cast<int>(other.format);
        if (blend != other.blend) return blend < other.blend;
        return writeMask < other.writeMask;
    }
};

/**
 * Index format enumeration
 * Defines the format of index buffer data
 */
enum struct IndexFormat {
    Undefined = 0x00000000,    ///< Undefined index format
    Uint16 = 0x00000001,        ///< 16-bit unsigned integer
    Uint32 = 0x00000002,        ///< 32-bit unsigned integer
};

/**
 * Polygon mode enumeration
 * Defines how polygons are rendered
 */
// enum struct PolygonMode {
//     Undefined = 0x00000000,    ///< Undefined polygon mode
//     Fill = 0x00000001,          ///< Fill polygons
//     Line = 0x00000002,           ///< Render polygons as lines
//     Point = 0x00000003,          ///< Render polygons as points
// };

/**
 * Primitive state
 * Represents the primitive state for rendering
 */
struct PrimitiveState {
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;  ///< Primitive topology
    IndexFormat stripIndexFormat = IndexFormat::Uint32;             ///< Strip index format
    FrontFace frontFace = FrontFace::CCW;                          ///< Front face winding
    CullMode cullMode = CullMode::None;                            ///< Cull mode
    bool unclippedDepth = false;                                   ///< Unclipped depth
    //PolygonMode polygonMode = PolygonMode::Fill;                    ///< Polygon mode
};

/**
 * Stencil operation enumeration
 * Defines stencil operations
 */
enum struct StencilOperation {
    Undefined = 0x00000000,     ///< Undefined stencil operation
    Keep = 0x00000001,           ///< Keep stencil value
    Zero = 0x00000002,           ///< Set stencil value to zero
    Replace = 0x00000003,         ///< Replace stencil value
    Invert = 0x00000004,          ///< Invert stencil value
    IncrementClamp = 0x00000005,  ///< Increment stencil value and clamp
    DecrementClamp = 0x00000006,  ///< Decrement stencil value and clamp
    IncrementWrap = 0x00000007,   ///< Increment stencil value and wrap
    DecrementWrap = 0x00000008,   ///< Decrement stencil value and wrap
};

/**
 * Stencil face state
 * Represents the stencil state for a single face
 */
struct StencilFaceState {
    CompareFunction compare;        ///< Compare function
    StencilOperation failOp;        ///< Operation when stencil test fails
    StencilOperation depthFailOp;   ///< Operation when depth test fails
    StencilOperation passOp;        ///< Operation when both tests pass
};

/**
 * Depth stencil state
 * Represents the depth and stencil state for rendering
 */
struct DepthStencilState {
    TextureFormat format;                ///< Depth stencil format
    bool depthTestEnabled = false;
    bool depthWriteEnabled = true;              ///< Whether depth writing is enabled
    CompareFunction depthCompare;        ///< Depth compare function

    bool stencilTestEnabled = false;
    StencilFaceState stencilFront;       ///< Front face stencil state
    StencilFaceState stencilBack;        ///< Back face stencil state
    uint32_t stencilReadMask;            ///< Stencil read mask
    uint32_t stencilWriteMask;           ///< Stencil write mask

    bool depthBiasEnabled = false;
    int32_t depthBias;                   ///< Depth bias
    float depthBiasSlopeScale;           ///< Depth bias slope scale
    float depthBiasClamp;                ///< Depth bias clamp

    bool operator==(const DepthStencilState& other) const {
        if (format != other.format) return false;
        if (depthTestEnabled != other.depthTestEnabled) return false;
        if (depthWriteEnabled != other.depthWriteEnabled) return false;
        if (depthCompare != other.depthCompare) return false;
        if (stencilTestEnabled != other.stencilTestEnabled) return false;
        if (stencilFront.compare != other.stencilFront.compare) return false;
        if (stencilFront.failOp != other.stencilFront.failOp) return false;
        if (stencilFront.depthFailOp != other.stencilFront.depthFailOp) return false;
        if (stencilFront.passOp != other.stencilFront.passOp) return false;
        if (stencilBack.compare != other.stencilBack.compare) return false;
        if (stencilBack.failOp != other.stencilBack.failOp) return false;
        if (stencilBack.depthFailOp != other.stencilBack.depthFailOp) return false;
        if (stencilBack.passOp != other.stencilBack.passOp) return false;
        if (stencilReadMask != other.stencilReadMask) return false;
        if (stencilWriteMask != other.stencilWriteMask) return false;
        if (depthBiasEnabled != other.depthBiasEnabled) return false;
        if (depthBias != other.depthBias) return false;
        if (depthBiasSlopeScale != other.depthBiasSlopeScale) return false;
        if (depthBiasClamp != other.depthBiasClamp) return false;
        return true;
    }

    bool operator!=(const DepthStencilState& other) const {
        return !(*this == other);
    }

    bool operator<(const DepthStencilState& other) const {
        if (format != other.format) return static_cast<int>(format) < static_cast<int>(other.format);
        if (depthTestEnabled != other.depthTestEnabled) return depthTestEnabled < other.depthTestEnabled;
        if (depthWriteEnabled != other.depthWriteEnabled) return depthWriteEnabled < other.depthWriteEnabled;
        if (depthCompare != other.depthCompare) return static_cast<int>(depthCompare) < static_cast<int>(other.depthCompare);
        if (stencilTestEnabled != other.stencilTestEnabled) return stencilTestEnabled < other.stencilTestEnabled;
        if (stencilFront.compare != other.stencilFront.compare) return static_cast<int>(stencilFront.compare) < static_cast<int>(other.stencilFront.compare);
        if (stencilFront.failOp != other.stencilFront.failOp) return static_cast<int>(stencilFront.failOp) < static_cast<int>(other.stencilFront.failOp);
        if (stencilFront.depthFailOp != other.stencilFront.depthFailOp) return static_cast<int>(stencilFront.depthFailOp) < static_cast<int>(other.stencilFront.depthFailOp);
        if (stencilFront.passOp != other.stencilFront.passOp) return static_cast<int>(stencilFront.passOp) < static_cast<int>(other.stencilFront.passOp);
        if (stencilBack.compare != other.stencilBack.compare) return static_cast<int>(stencilBack.compare) < static_cast<int>(other.stencilBack.compare);
        if (stencilBack.failOp != other.stencilBack.failOp) return static_cast<int>(stencilBack.failOp) < static_cast<int>(other.stencilBack.failOp);
        if (stencilBack.depthFailOp != other.stencilBack.depthFailOp) return static_cast<int>(stencilBack.depthFailOp) < static_cast<int>(other.stencilBack.depthFailOp);
        if (stencilBack.passOp != other.stencilBack.passOp) return static_cast<int>(stencilBack.passOp) < static_cast<int>(other.stencilBack.passOp);
        if (stencilReadMask != other.stencilReadMask) return stencilReadMask < other.stencilReadMask;
        if (stencilWriteMask != other.stencilWriteMask) return stencilWriteMask < other.stencilWriteMask;
        if (depthBiasEnabled != other.depthBiasEnabled) return depthBiasEnabled < other.depthBiasEnabled;
        if (depthBias != other.depthBias) return depthBias < other.depthBias;
        if (depthBiasSlopeScale != other.depthBiasSlopeScale) return depthBiasSlopeScale < other.depthBiasSlopeScale;
        return depthBiasClamp < other.depthBiasClamp;
    }
};

/**
 * Multisample state
 * Represents the multisample state for rendering
 */
struct MultisampleState {
    uint32_t count;                 ///< Sample count
    uint32_t mask;                  ///< Sample mask
    bool alphaToCoverageEnabled;    ///< Whether alpha to coverage is enabled
};

/**
 * Pipeline descriptor
 * Contains all properties needed to create a pipeline
 */
struct PipelineDesc {
    const char* label = nullptr;                ///< Optional label for the pipeline
    Shader* vertexShader;                       ///< Vertex shader
    Shader* fragmentShader;                     ///< Fragment shader

    std::vector<BufferLayout> bufferLayout;     ///< Buffer layouts for vertex attributes

    PrimitiveState primitive;                   ///< Primitive state

    DepthStencilState depthStencil;  ///< Depth stencil state

    std::vector<ColorTargetState> targets;      ///< Color target states

    MultisampleState multisample = {            ///< Multisample state
        1, 0xffffffff, false
    };

    bool operator==(const PipelineDesc& other) const {
        if (label != other.label) return false;
        if (vertexShader != other.vertexShader) return false;
        if (fragmentShader != other.fragmentShader) return false;
        if (bufferLayout != other.bufferLayout) return false;
        if (memcmp(&primitive, &other.primitive, sizeof(PrimitiveState)) != 0) return false;
        if (depthStencil != other.depthStencil) return false;
        if (targets != other.targets) return false;
        if (memcmp(&multisample, &other.multisample, sizeof(MultisampleState)) != 0) return false;
        return true;
    }

    bool operator<(const PipelineDesc& other) const {
        if (label != other.label) return reinterpret_cast<uintptr_t>(label) < reinterpret_cast<uintptr_t>(other.label);
        if (vertexShader != other.vertexShader) return reinterpret_cast<uintptr_t>(vertexShader) < reinterpret_cast<uintptr_t>(other.vertexShader);
        if (fragmentShader != other.fragmentShader) return reinterpret_cast<uintptr_t>(fragmentShader) < reinterpret_cast<uintptr_t>(other.fragmentShader);
        if (bufferLayout != other.bufferLayout) return bufferLayout < other.bufferLayout;
        if (memcmp(&primitive, &other.primitive, sizeof(PrimitiveState)) != 0) {
            return memcmp(&primitive, &other.primitive, sizeof(PrimitiveState)) < 0;
        }
        if (depthStencil != other.depthStencil) return depthStencil < other.depthStencil;
        if (targets != other.targets) return targets < other.targets;
        return memcmp(&multisample, &other.multisample, sizeof(MultisampleState)) < 0;
    }
};

/**
 * Pipeline interface
 * Represents a graphics pipeline
 */
struct Pipeline {
    ShaderReflection reflection;                ///< Shader reflection information
    virtual ~Pipeline() {}
protected:
    /**
     * Reflect on the pipeline descriptor
     * @param desc Pipeline descriptor to reflect on
     */
    void reflect(const PipelineDesc* desc);
};

/**
 * Composite alpha mode enumeration
 * Defines the composite alpha mode for a surface
 */
enum struct CompositeAlphaMode {
    Auto = 0x00000000,           ///< Auto alpha mode
    Opaque = 0x00000001,          ///< Opaque alpha mode
    Premultiplied = 0x00000002,   ///< Premultiplied alpha mode
    Unpremultiplied = 0x00000003, ///< Unpremultiplied alpha mode
    Inherit = 0x00000004,         ///< Inherit alpha mode
};

/**
 * Surface descriptor
 * Contains all properties needed to create a surface
 */
struct SurfaceDesc {
    int width;                              ///< Surface width
    int height;                             ///< Surface height
    TextureFormat format = TextureFormat::BGRA8Unorm; ///< Surface format
    CompositeAlphaMode alphaMode = CompositeAlphaMode::Opaque; ///< Alpha mode
    void* surfaceChain = nullptr;           ///< Platform-specific surface chain
};

struct CommandEncoder;
struct FrameBuffer;

/**
 * Surface interface
 * Represents a render surface
 */
struct Surface {
    /**
     * Resize the surface
     * @param w New width
     * @param h New height
     */
    virtual void resize(int w, int h) = 0;
    
    virtual ~Surface() {}
    
    /**
     * Acquire the next image
     * @return True if the image was successfully acquired
     */
    virtual bool nextImage() = 0;
    
    /**
     * Present the current image
     */
    virtual void present() = 0;

    /**
     * Get the current texture view
     * @return Pointer to the current texture view
     */
    virtual APtr<Texture> getCurTextureView() = 0;
    
    /**
     * Get the current command encoder
     * @return Pointer to the current command encoder
     */
    virtual CommandEncoder* getCurCommandEncoder() = 0;
    
    /**
     * Get the current frame buffer
     * @return Pointer to the current frame buffer
     */
    virtual APtr<FrameBuffer> getCurFrameBuffer() = 0;
    
    /**
     * Cache a frame buffer
     * @param fbo Frame buffer to cache
     */
    virtual void cacheFrameBuffer(APtr<FrameBuffer> fbo) = 0;
};


}

namespace std {
    template<>
    struct hash<arhi::AttributeElement> {
        size_t operator()(const arhi::AttributeElement& elem) const {
            size_t h = 0;
            hash_combine(h, elem.name);
            hash_combine(h, elem.offset);
            hash_combine(h, elem._location);
            hash_combine(h, elem._format);
            hash_combine(h, elem._size);
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::BlendComponent> {
        size_t operator()(const arhi::BlendComponent& comp) const {
            size_t h = 0;
            hash_combine(h, static_cast<int>(comp.operation));
            hash_combine(h, static_cast<int>(comp.srcFactor));
            hash_combine(h, static_cast<int>(comp.dstFactor));
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::BlendState> {
        size_t operator()(const arhi::BlendState& state) const {
            size_t h = 0;
            hash_combine(h, state.color);
            hash_combine(h, state.alpha);
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::StencilFaceState> {
        size_t operator()(const arhi::StencilFaceState& state) const {
            size_t h = 0;
            hash_combine(h, static_cast<int>(state.compare));
            hash_combine(h, static_cast<int>(state.failOp));
            hash_combine(h, static_cast<int>(state.depthFailOp));
            hash_combine(h, static_cast<int>(state.passOp));
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::DepthStencilState> {
        size_t operator()(const arhi::DepthStencilState& state) const {
            size_t h = 0;
            hash_combine(h, static_cast<int>(state.format));
            hash_combine(h, state.depthTestEnabled);
            hash_combine(h, state.depthWriteEnabled);
            hash_combine(h, static_cast<int>(state.depthCompare));
            hash_combine(h, state.stencilTestEnabled);
            hash_combine(h, state.stencilFront);
            hash_combine(h, state.stencilBack);
            hash_combine(h, state.stencilReadMask);
            hash_combine(h, state.stencilWriteMask);
            hash_combine(h, state.depthBiasEnabled);
            hash_combine(h, state.depthBias);
            hash_combine(h, state.depthBiasSlopeScale);
            hash_combine(h, state.depthBiasClamp);
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::BufferLayout> {
        size_t operator()(const arhi::BufferLayout& layout) const {
            size_t h = 0;
            hash_combine(h, layout.stride);
            hash_combine(h, static_cast<int>(layout.stepMode));
            for (const auto& elem : layout.elements) {
                hash_combine(h, elem);
            }
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::ColorTargetState> {
        size_t operator()(const arhi::ColorTargetState& state) const {
            size_t h = 0;
            hash_combine(h, static_cast<int>(state.format));
            hash_combine(h, state.blend);
            hash_combine(h, state.writeMask);
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::PipelineDesc> {
        size_t operator()(const arhi::PipelineDesc& desc) const {
            size_t h = 0;
            hash_combine(h, reinterpret_cast<uintptr_t>(desc.label));
            hash_combine(h, reinterpret_cast<uintptr_t>(desc.vertexShader));
            hash_combine(h, reinterpret_cast<uintptr_t>(desc.fragmentShader));
            for (const auto& layout : desc.bufferLayout) {
                hash_combine(h, layout.stride);
                hash_combine(h, static_cast<int>(layout.stepMode));
                for (const auto& elem : layout.elements) {
                    hash_combine(h, elem.name);
                    hash_combine(h, elem.offset);
                    hash_combine(h, elem._location);
                    hash_combine(h, elem._format);
                    hash_combine(h, elem._size);
                }
            }
            hash_combine(h, static_cast<int>(desc.primitive.topology));
            hash_combine(h, static_cast<int>(desc.primitive.stripIndexFormat));
            hash_combine(h, static_cast<int>(desc.primitive.frontFace));
            hash_combine(h, static_cast<int>(desc.primitive.cullMode));
            hash_combine(h, desc.primitive.unclippedDepth);
            //hash_combine(h, static_cast<int>(desc.primitive.polygonMode));
            hash_combine(h, desc.depthStencil);
            for (const auto& target : desc.targets) {
                hash_combine(h, static_cast<int>(target.format));
                hash_combine(h, target.blend);
                hash_combine(h, target.writeMask);
            }
            hash_combine(h, desc.multisample.count);
            hash_combine(h, desc.multisample.mask);
            hash_combine(h, desc.multisample.alphaToCoverageEnabled);
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}

#endif