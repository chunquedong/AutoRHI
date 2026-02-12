#ifndef Shader_H_
#define Shader_H_

#include "rhi_common.h"


namespace arhi {

enum struct ShaderStage {
    Vertex = 1,
    Fragment = 1 << 1,
    Compute = 1 << 2,
    Geometry = 1 << 5,
};

struct AttributeVar {
    std::string name;
    int location;
    int format;
    int size;
    //int componentCount;
    //int bytePerComponent;
    //bool isFloat = true;
    //bool isSigned = true;
};

enum struct UniformType {
    Unknow,
    Texture,
    Sampler,
    UniformBuffer,
    StorageBuffer
};

struct UniformVar {
    std::string name;
    int binding;
    int bindGroup;
    //int glIndex;
    int glLocation;
    UniformType type;

    int textureSampleType;
    int textureViewDimension;
    int samplerBindingType;

    int bufferBindingType;
    int bufferMinBindingSize;

    int visibility;
};

struct ShaderReflection {
    std::map<std::string, UniformVar> uniforms;
    std::map<int, AttributeVar> attributes;
    std::map<std::string, AttributeVar> attributesIndex;
};

struct ShaderModuleReflection {
    std::vector<UniformVar> uniforms;
    std::vector<AttributeVar> inputAttrs;
    std::vector<AttributeVar> outputAttrs;
};

enum struct ShaderLanguage {
    SPIRV = 0x00000001,
    WGSL = 0x00000002,
    GLSL = 0x10000003,
};

struct ShaderDesc {
    const char* source;
    int sourceLen = -1;
    const char* defines = nullptr;
    const char* label = nullptr;
    ShaderStage stage;
    ShaderLanguage language;
};

struct Shader {
    const char* entryPoint = "main";
    ShaderModuleReflection reflection;
    virtual ~Shader() {}
};


}

#endif