/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef Shader_H_
#define Shader_H_

#include "rhi_common.h"


namespace arhi {

/**
 * Shader stage enumeration
 * Defines different shader stages
 */
enum struct ShaderStage {
    Vertex = 1,        ///< Vertex shader stage
    Fragment = 1 << 1, ///< Fragment shader stage
    Compute = 1 << 2,  ///< Compute shader stage
    Geometry = 1 << 5, ///< Geometry shader stage
};

enum struct VertexFormat {
    Uint8 = 0x00000001,
    Uint8x2 = 0x00000002,
    Uint8x4 = 0x00000003,
    Sint8 = 0x00000004,
    Sint8x2 = 0x00000005,
    Sint8x4 = 0x00000006,
    Unorm8 = 0x00000007,
    Unorm8x2 = 0x00000008,
    Unorm8x4 = 0x00000009,
    Snorm8 = 0x0000000A,
    Snorm8x2 = 0x0000000B,
    Snorm8x4 = 0x0000000C,
    Uint16 = 0x0000000D,
    Uint16x2 = 0x0000000E,
    Uint16x4 = 0x0000000F,
    Sint16 = 0x00000010,
    Sint16x2 = 0x00000011,
    Sint16x4 = 0x00000012,
    Unorm16 = 0x00000013,
    Unorm16x2 = 0x00000014,
    Unorm16x4 = 0x00000015,
    Snorm16 = 0x00000016,
    Snorm16x2 = 0x00000017,
    Snorm16x4 = 0x00000018,
    Float16 = 0x00000019,
    Float16x2 = 0x0000001A,
    Float16x4 = 0x0000001B,
    Float32 = 0x0000001C,
    Float32x2 = 0x0000001D,
    Float32x3 = 0x0000001E,
    Float32x4 = 0x0000001F,
    Uint32 = 0x00000020,
    Uint32x2 = 0x00000021,
    Uint32x3 = 0x00000022,
    Uint32x4 = 0x00000023,
    Sint32 = 0x00000024,
    Sint32x2 = 0x00000025,
    Sint32x3 = 0x00000026,
    Sint32x4 = 0x00000027,
    Unorm10_10_10_2 = 0x00000028,
    Unorm8x4BGRA = 0x00000029,
};

/**
 * Attribute variable
 * Represents a vertex attribute variable
 */
struct AttributeVar {
    std::string name;   ///< Name of the attribute
    int location;       ///< Location of the attribute
    VertexFormat format;         ///< Format of the attribute
    int size;           ///< Size of the attribute
    int specificFormat; ///< Platform-specific Format
};

/**
 * Uniform type enumeration
 * Defines different types of uniform variables
 */
enum struct UniformType {
    Unknow,          ///< Unknown uniform type
    Texture,         ///< Texture uniform
    Sampler,         ///< Sampler uniform
    UniformBuffer,   ///< Uniform buffer
    StorageBuffer    ///< Storage buffer
};

/**
 * Uniform variable
 * Represents a uniform variable in a shader
 */
struct UniformVar {
    std::string name;            ///< Name of the uniform
    int binding;                 ///< Binding index
    int bindGroup;               ///< Binding group
    int glLocation;              ///< OpenGL location
    UniformType type;            ///< Type of the uniform

    int textureSampleType;       ///< Texture sample type
    int textureViewDimension;    ///< Texture view dimension
    int samplerBindingType;      ///< Sampler binding type

    int bufferBindingType;       ///< Buffer binding type
    int bufferMinBindingSize;    ///< Minimum buffer binding size

    int visibility;              ///< Visibility mask
};

/**
 * Shader reflection
 * Contains reflection information for a shader
 */
struct ShaderReflection {
    std::map<std::string, UniformVar> uniforms;         ///< Map of uniform variables by name
    std::map<int, AttributeVar> attributes;             ///< Map of attributes by location
    std::map<std::string, AttributeVar> attributesIndex; ///< Map of attributes by name
};

/**
 * Shader module reflection
 * Contains reflection information for a shader module
 */
struct ShaderModuleReflection {
    std::vector<UniformVar> uniforms;     ///< List of uniform variables
    std::vector<AttributeVar> inputAttrs;  ///< List of input attributes
    std::vector<AttributeVar> outputAttrs; ///< List of output attributes
};

/**
 * Shader language enumeration
 * Defines different shader languages
 */
enum struct ShaderLanguage {
    SPIRV = 0x00000001,   ///< SPIR-V shader language
    WGSL = 0x00000002,    ///< WebGPU Shading Language
    GLSL = 0x10000003,     ///< OpenGL Shading Language
};

/**
 * Shader descriptor
 * Contains all properties needed to create a shader
 */
struct ShaderDesc {
    const char* source;         ///< Shader source code
    int sourceLen = -1;         ///< Length of the source code (-1 for null-terminated)
    const char* defines = nullptr; ///< Preprocessor defines
    const char* label = nullptr;   ///< Optional label for the shader
    ShaderStage stage;          ///< Shader stage
    ShaderLanguage language;     ///< Shader language
};

/**
 * Shader interface
 * Represents a shader resource
 */
struct Shader {
    const char* entryPoint = "main"; ///< Entry point function name
    ShaderModuleReflection reflection; ///< Reflection information for the shader
    virtual ~Shader() {}
};


}

#endif