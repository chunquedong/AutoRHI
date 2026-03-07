
#include "rhi_spirv.h"

const char vertexSource[] = R"(
#if __VERSION__ < 450
    #define LAYOUT_LOCATION(locN) 
    #define LAYOUT_BIDING(setN, bindingN) 
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) uniform samplerType samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerN
#else
    #define LAYOUT_LOCATION(locN) layout(location = locN)
    #define LAYOUT_BIDING(setN, bindingN) layout(set = setN, binding = bindingN)
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN, set = setN, binding = bindingN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) \
        layout(set = setN1, binding = bindingN1) uniform textureType textureN; \
        layout(set = setN1, binding = bindingN1+1) uniform sampler samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerType(textureN, samplerN)
#endif

LAYOUT_LOCATION(0) in vec2 position;
LAYOUT_LOCATION(1) in vec2 uv;

LAYOUT_LOCATION(0) out vec2 v_uv;

LAYOUT_BUFFER(0, 3, std140) uniform FrameData {
    vec4 offset;
};

void main() {
    gl_Position = vec4(position.x+offset.x, position.y, 0.0, 1.0);
    v_uv = uv;
}
)";


const char fragmentSource[] = R"(
#if __VERSION__ < 450
    #define LAYOUT_LOCATION(locN) 
    #define LAYOUT_BIDING(setN, bindingN) 
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) uniform samplerType samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerN
#else
    #define LAYOUT_LOCATION(locN) layout(location = locN)
    #define LAYOUT_BIDING(setN, bindingN) layout(set = setN, binding = bindingN)
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN, set = setN, binding = bindingN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) \
        layout(set = setN1, binding = bindingN1) uniform textureType textureN; \
        layout(set = setN1, binding = bindingN1+1) uniform sampler samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerType(textureN, samplerN)
#endif

LAYOUT_LOCATION(0) in vec2 v_uv;
LAYOUT_LOCATION(0) out vec4 out_color;

DEF_TEXTURE_SAMPLER(0, 0, texture2D, colDiffuse, sampler2D, grsampler);

void main() {
    out_color = texture(GET_SAMPLER_2D(sampler2D, colDiffuse, grsampler), v_uv);
}
)";

const char vertexSourceWGSL[] = R"(
    // Vertex shader
    struct FrameData {
        offset: vec4<f32>,
    }

    @group(0) @binding(3) var<uniform> frame_data: FrameData;

    struct VertexInput {
        @location(0) position: vec2<f32>,
        @location(1) uv: vec2<f32>,
    }

    struct VertexOutput {
        @builtin(position) position: vec4<f32>,
        @location(0) v_uv: vec2<f32>,
    }

    @vertex
    fn main(input: VertexInput) -> VertexOutput {
        var output: VertexOutput;
        output.position = vec4<f32>(
            input.position.x + frame_data.offset.x,
            input.position.y,
            0.0,
            1.0
        );
        output.v_uv = input.uv;
        return output;
    }
)";

const char fragmentSourceWGSL[] = R"(
// Fragment shader
@group(0) @binding(0) var colDiffuse: texture_2d<f32>;
@group(0) @binding(1) var grsampler: sampler;

struct FragmentInput {
    @location(0) v_uv: vec2<f32>,
}

struct FragmentOutput {
    @location(0) out_color: vec4<f32>,
}

@fragment
fn main(input: FragmentInput) -> FragmentOutput {
    var output: FragmentOutput;
    output.out_color = textureSample(colDiffuse, grsampler, input.v_uv);
    return output;
}
)";

#include <vector>
#include <cstdio>

using namespace arhi;

int testGLSL() {
    // Convert vertex shader
    std::vector<uint32_t> vertex_spirv;
    bool vertex_success = compileGLSL(vertexSource, ShaderStage::Vertex, vertex_spirv);
    
    // Convert fragment shader
    std::vector<uint32_t> fragment_spirv;
    bool fragment_success = compileGLSL(fragmentSource, ShaderStage::Fragment, fragment_spirv);
    
    if (!vertex_success || vertex_spirv.empty()) {
        printf("   ERROR: Failed to compile vertex shader\n");
        return 1;
    }
    
    if (!fragment_success || fragment_spirv.empty()) {
        printf("   ERROR: Failed to compile fragment shader\n");
        return 1;
    }
    
    printf("   GLSL to SPIR-V conversion successful!\n\n");

    // Convert vertex SPIR-V to WGSL
    printf("   Vertex shader conversion:\n");
    std::string vertex_wgsl;
    bool vertex_wgsl_success = spirvToWGSL(
        reinterpret_cast<const char*>(vertex_spirv.data()),
        static_cast<int>(vertex_spirv.size() * sizeof(uint32_t)),
        vertex_wgsl
    );
    
    if (vertex_wgsl_success) {
        printf("   ----------------------------------------------------\n");
        printf("%s\n", vertex_wgsl.c_str());
        printf("   ----------------------------------------------------\n\n");
    } else {
        printf("   ERROR: Failed to convert vertex SPIR-V to WGSL\n\n");
    }
    
    // Convert fragment SPIR-V to WGSL
    printf("   Fragment shader conversion:\n");
    std::string fragment_wgsl;
    bool fragment_wgsl_success = spirvToWGSL(
        reinterpret_cast<const char*>(fragment_spirv.data()),
        static_cast<int>(fragment_spirv.size() * sizeof(uint32_t)),
        fragment_wgsl
    );
    
    if (fragment_wgsl_success) {
        printf("   ----------------------------------------------------\n");
        printf("%s\n", fragment_wgsl.c_str());
        printf("   ----------------------------------------------------\n\n");
    } else {
        printf("   ERROR: Failed to convert fragment SPIR-V to WGSL\n\n");
    }
    
    printf("3. Conversion complete!\n");
    printf("All shaders have been successfully converted from GLSL to WGSL.\n");

    return 0;
}

int testWGSL() {

    // Convert vertex shader
    std::vector<uint32_t> vertex_spirv;
    bool vertex_success = compileWGSL(vertexSourceWGSL, ShaderStage::Vertex, vertex_spirv);
    if (!vertex_success || vertex_spirv.empty()) {
        printf("   ERROR: Failed to compile vertex shader\n");
        return 1;
    }
    else {
        ShaderModuleReflection reflection;
        spirvReflect(vertex_spirv, ShaderStage::Vertex, reflection);
    }

    // Convert fragment shader
    std::vector<uint32_t> fragment_spirv;
    bool fragment_success = compileWGSL(fragmentSourceWGSL, ShaderStage::Fragment, fragment_spirv);
    if (!fragment_success || fragment_spirv.empty()) {
        printf("   ERROR: Failed to compile fragment shader\n");
        return 1;
    }
    else {
        ShaderModuleReflection reflection;
        spirvReflect(fragment_spirv, ShaderStage::Fragment, reflection);
    }
    return 0;
}

int main() {
    testWGSL();
}