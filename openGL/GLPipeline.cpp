#include "GLPipeline.h"
#include "GLShader.h"

#include <map>

using namespace arhi;

void GLPipeline::reflect(const PipelineDesc* desc) {
    
    GLint activeAttributes;
    GL_ASSERT(glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttributes));
    if (activeAttributes > 0)
    {
        GLchar attribName[128] = {};
        GLint attribSize;
        GLenum attribType;
        GLint attribLocation;
        GLint length = 128;
        for (int i = 0; i < activeAttributes; ++i)
        {
            // Query attribute info.
            GL_ASSERT(glGetActiveAttrib(program, i, length, NULL, &attribSize, &attribType, attribName));

            // Query the pre-assigned attribute location.
            GL_ASSERT(attribLocation = glGetAttribLocation(program, attribName));

            AttributeVar avar;
            avar.specificFormat = attribType;
            avar.size = attribSize;
            avar.location = attribLocation;
            avar.name = attribName;

            reflection.attributesIndex[attribName] = avar;
            reflection.attributes[attribLocation] = avar;
        }
    }

    // Query and store uniforms from the program.
    GLint activeUniforms;
    GL_ASSERT(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUniforms));
    if (activeUniforms > 0)
    {

        GLchar uniformName[128] = {};
        GLint uniformSize;
        GLenum uniformType;
        GLint uniformLocation;
        GLint length = 128;
        unsigned int samplerIndex = 0;
        for (int i = 0; i < activeUniforms; ++i)
        {
            // Query uniform info.
            GL_ASSERT(glGetActiveUniform(program, i, length, NULL, &uniformSize, &uniformType, uniformName));
            if (strlen(uniformName) > 3)
            {
                // If this is an array uniform, strip array indexers off it since GL does not
                // seem to be consistent across different drivers/implementations in how it returns
                // array uniforms. On some systems it will return "u_matrixArray", while on others
                // it will return "u_matrixArray[0]".
                char* c = strrchr(uniformName, '[');
                if (c)
                {
                    *c = '\0';
                }
            }

            UniformVar uniform = {};

            if (uniformType == GL_SAMPLER_2D || uniformType == GL_SAMPLER_CUBE)
            {
                uniform.binding = samplerIndex;
                samplerIndex += uniformSize;
                uniform.type = UniformType::Sampler;
            }
            else
            {
                continue;
            }

            // Query the pre-assigned uniform location.
            GL_ASSERT(uniformLocation = glGetUniformLocation(program, uniformName));

            uniform.name = uniformName;
            uniform.glLocation = uniformLocation;
            uniform.bufferBindingType = uniformType;
            uniform.bufferMinBindingSize = uniformSize;

            reflection.uniforms[uniform.name] = uniform;
        }
    }

    GLint activeUniformBlock;
    GL_ASSERT(glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &activeUniformBlock));
    if (activeUniformBlock > 0)
    {

        GLchar uniformName[128] = {};
        GLint uniformSize;
        GLint uniformBinding = 0;
        GLint uniformBlockIndex;
        GLint length = 128;
        unsigned int samplerIndex = 0;
        for (int i = 0; i < activeUniformBlock; ++i)
        {
            // Query uniform info.
            GL_ASSERT(glGetActiveUniformBlockName(program, i, length, NULL, uniformName));
            if (strlen(uniformName) > 3)
            {
                // If this is an array uniform, strip array indexers off it since GL does not
                // seem to be consistent across different drivers/implementations in how it returns
                // array uniforms. On some systems it will return "u_matrixArray", while on others
                // it will return "u_matrixArray[0]".
                char* c = strrchr(uniformName, '[');
                if (c)
                {
                    *c = '\0';
                }
            }

            UniformVar uniform = {};

            // Query the pre-assigned uniform location.
            GL_ASSERT(uniformBlockIndex = glGetUniformBlockIndex(program, uniformName));

            GL_ASSERT(glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformSize));
            //GL_ASSERT(glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_BINDING, &uniformBinding));

            uniform.name = uniformName;
            uniform.binding = uniformBinding;
            uniform.type = UniformType::UniformBuffer;
            uniform.bufferBindingType = GL_UNIFORM_BUFFER;
            uniform.bufferMinBindingSize = uniformSize;

            GL_ASSERT(glUniformBlockBinding(program, uniformBlockIndex, uniform.binding));
            reflection.uniforms[uniform.name] = uniform;

            ++uniformBinding;
        }
    }

    
}

void GLPipeline::buildBindGroupLayoutList(GLDevice* adevice) {

}

APtr<GLPipeline> GLPipeline::create(GLDevice* adevice, const PipelineDesc* desc) {
    GLuint vertexShader = dynamic_cast<GLShader*>(desc->vertexShader)->shaderModule;
    GLuint fragmentShader = dynamic_cast<GLShader*>(desc->fragmentShader)->shaderModule;
    // Link program.
    GLuint program;
    GL_ASSERT(program = glCreateProgram());
    GL_ASSERT(glAttachShader(program, vertexShader));
    GL_ASSERT(glAttachShader(program, fragmentShader));
    GL_ASSERT(glLinkProgram(program));
    GLint success;
    GL_ASSERT(glGetProgramiv(program, GL_LINK_STATUS, &success));

    // Check link status.
    if (success != GL_TRUE)
    {
        GLint length;
        GL_ASSERT(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        if (length == 0)
        {
            length = 4096;
        }
        char* infoLog = NULL;
        if (length > 0)
        {
            infoLog = new char[length];
            GL_ASSERT(glGetProgramInfoLog(program, length, NULL, infoLog));
            infoLog[length - 1] = '\0';
        }
        ARHI_ERROR("Linking program failed (%s): %s", desc->label, infoLog == NULL ? "" : infoLog);
        delete[](infoLog);

        // Clean up.
        GL_ASSERT(glDeleteProgram(program));

        program = 0;
        return nullptr;
    }

    auto pipeline = makeAPtr<GLPipeline>();
    pipeline->program = program;
    pipeline->reflect(desc);
    pipeline->desc = *desc;

    for (BufferLayout& layout : pipeline->desc.bufferLayout) {
        for (AttributeElement& elem : layout.elements) {
            auto shaderFound = pipeline->reflection.attributesIndex.find(elem.name);
            if (shaderFound == pipeline->reflection.attributesIndex.end()) {
                printf("Not found attribute %s\n", elem.name.c_str());
                continue;
            }
            elem._format = shaderFound->second.specificFormat;
            elem._location = shaderFound->second.location;
            elem._size = shaderFound->second.size;
        }
    }
    return pipeline;
}

GLPipeline::~GLPipeline() {
    if (program && GraphicsDevice::cur()) {
        GL_ASSERT(glDeleteProgram(program));
        program = 0;
    }
}

void arhi::GLPipeline::applyState()
{
    this->applyPrimitiveState();
    this->applyDepthStencilState();
    this->applyBlendState();
    //this->applyMultisampleState();
}

// Convert PrimitiveTopology to GL enum
GLenum getGLPrimitiveTopology(PrimitiveTopology topology) {
    switch (topology) {
    case PrimitiveTopology::PointList:
        return GL_POINTS;
    case PrimitiveTopology::LineList:
        return GL_LINES;
    case PrimitiveTopology::LineStrip:
        return GL_LINE_STRIP;
    case PrimitiveTopology::TriangleList:
        return GL_TRIANGLES;
    case PrimitiveTopology::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    default:
        return GL_TRIANGLES;
    }
}

// Convert FrontFace to GL enum
GLenum getGLFrontFace(FrontFace frontFace) {
    switch (frontFace) {
    case FrontFace::CCW:
        return GL_CCW;
    case FrontFace::CW:
        return GL_CW;
    default:
        return GL_CCW;
    }
}

// Convert CullMode to GL enum
GLenum getGLCullMode(CullMode cullMode) {
    switch (cullMode) {
    case CullMode::None:
        return GL_NONE;
    case CullMode::Front:
        return GL_FRONT;
    case CullMode::Back:
        return GL_BACK;
    default:
        return GL_BACK;
    }
}

// Convert PolygonMode to GL enum
// GLenum getGLPolygonMode(PolygonMode polygonMode) {
//     switch (polygonMode) {
//     case PolygonMode::Fill:
//         return GL_FILL;
//     case PolygonMode::Line:
//         return GL_LINE;
//     case PolygonMode::Point:
//         return GL_POINT;
//     default:
//         return GL_FILL;
//     }
// }

// Convert CompareFunction to GL enum
GLenum getGLCompareFunction(CompareFunction compare) {
    switch (compare) {
    case CompareFunction::Never:
        return GL_NEVER;
    case CompareFunction::Less:
        return GL_LESS;
    case CompareFunction::Equal:
        return GL_EQUAL;
    case CompareFunction::LessEqual:
        return GL_LEQUAL;
    case CompareFunction::Greater:
        return GL_GREATER;
    case CompareFunction::NotEqual:
        return GL_NOTEQUAL;
    case CompareFunction::GreaterEqual:
        return GL_GEQUAL;
    case CompareFunction::Always:
        return GL_ALWAYS;
    default:
        return GL_LESS;
    }
}

// Convert StencilOperation to GL enum
GLenum getGLStencilOperation(StencilOperation op) {
    switch (op) {
    case StencilOperation::Keep:
        return GL_KEEP;
    case StencilOperation::Zero:
        return GL_ZERO;
    case StencilOperation::Replace:
        return GL_REPLACE;
    case StencilOperation::Invert:
        return GL_INVERT;
    case StencilOperation::IncrementClamp:
        return GL_INCR;
    case StencilOperation::DecrementClamp:
        return GL_DECR;
    case StencilOperation::IncrementWrap:
        return GL_INCR_WRAP;
    case StencilOperation::DecrementWrap:
        return GL_DECR_WRAP;
    default:
        return GL_KEEP;
    }
}

// Convert BlendOperation to GL enum
GLenum getGLBlendOperation(BlendOperation op) {
    switch (op) {
    case BlendOperation::Add:
        return GL_FUNC_ADD;
    case BlendOperation::Subtract:
        return GL_FUNC_SUBTRACT;
    case BlendOperation::ReverseSubtract:
        return GL_FUNC_REVERSE_SUBTRACT;
    case BlendOperation::Min:
        return GL_MIN;
    case BlendOperation::Max:
        return GL_MAX;
    default:
        return GL_FUNC_ADD;
    }
}

// Convert BlendFactor to GL enum
GLenum getGLBlendFactor(BlendFactor factor) {
    switch (factor) {
    case BlendFactor::Zero:
        return GL_ZERO;
    case BlendFactor::One:
        return GL_ONE;
    case BlendFactor::Src:
        return GL_SRC_COLOR;
    case BlendFactor::OneMinusSrc:
        return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::SrcAlpha:
        return GL_SRC_ALPHA;
    case BlendFactor::OneMinusSrcAlpha:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::Dst:
        return GL_DST_COLOR;
    case BlendFactor::OneMinusDst:
        return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::DstAlpha:
        return GL_DST_ALPHA;
    case BlendFactor::OneMinusDstAlpha:
        return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::SrcAlphaSaturated:
        return GL_SRC_ALPHA_SATURATE;
    case BlendFactor::Constant:
        return GL_CONSTANT_COLOR;
    case BlendFactor::OneMinusConstant:
        return GL_ONE_MINUS_CONSTANT_COLOR;
#ifndef GLAD
    case BlendFactor::Src1:
        return GL_SRC1_COLOR;
    case BlendFactor::OneMinusSrc1:
        return GL_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::Src1Alpha:
        return GL_SRC1_ALPHA;
    case BlendFactor::OneMinusSrc1Alpha:
        return GL_ONE_MINUS_SRC1_ALPHA;
#endif
    default:
        return GL_ONE;
    }
}

// Apply PrimitiveState
void GLPipeline::applyPrimitiveState() {
    const PrimitiveState& primitive = desc.primitive;
    
    // Set primitive topology
    // Note: OpenGL sets topology during drawing, here we just store information
    
    // Set front face
    GL_ASSERT(glFrontFace(getGLFrontFace(primitive.frontFace)));
    
    // Set cull mode
    if (primitive.cullMode == CullMode::None) {
        GL_ASSERT(glDisable(GL_CULL_FACE));
    } else {
        GL_ASSERT(glEnable(GL_CULL_FACE));
        GL_ASSERT(glCullFace(getGLCullMode(primitive.cullMode)));
    }
    
    // Set polygon mode
    //GL_ASSERT(glPolygonMode(GL_FRONT_AND_BACK, getGLPolygonMode(primitive.polygonMode)));
    
    // Set depth bias
    if (desc.depthStencil) {
        GL_ASSERT(glPolygonOffset(desc.depthStencil->depthBiasSlopeScale, desc.depthStencil->depthBias));
    }
}

// Apply DepthStencilState
void GLPipeline::applyDepthStencilState() {
    if (desc.depthStencil) {
        const DepthStencilState& depthStencil = *desc.depthStencil;
        
        if (depthStencil.depthCompare != CompareFunction::Always) {
            // Enable depth test
            GL_ASSERT(glEnable(GL_DEPTH_TEST));

            // Set depth write
            GL_ASSERT(glDepthMask(depthStencil.depthWriteEnabled ? GL_TRUE : GL_FALSE));
            
            // Set depth compare function
            GL_ASSERT(glDepthFunc(getGLCompareFunction(depthStencil.depthCompare)));
            
            // Set depth bias
            GL_ASSERT(glPolygonOffset(depthStencil.depthBiasSlopeScale, depthStencil.depthBias));
        }
        else {
            GL_ASSERT(glDisable(GL_DEPTH_TEST));
        }
        
        // Enable stencil test
        if (depthStencil.stencilFront.compare != CompareFunction::Always 
            || depthStencil.stencilFront.passOp != arhi::StencilOperation::Keep
            || depthStencil.stencilFront.failOp != arhi::StencilOperation::Keep
            || depthStencil.stencilBack.compare != CompareFunction::Always
            || depthStencil.stencilBack.passOp != arhi::StencilOperation::Keep
            || depthStencil.stencilBack.failOp != arhi::StencilOperation::Keep) {
            GL_ASSERT(glEnable(GL_STENCIL_TEST));
            
            // Set stencil read/write mask
            GL_ASSERT(glStencilMask(depthStencil.stencilWriteMask));
            
            // Set front face stencil state
            GL_ASSERT(glStencilFuncSeparate(GL_FRONT, 
                                           getGLCompareFunction(depthStencil.stencilFront.compare), 
                                           0, 
                                           depthStencil.stencilReadMask));
            GL_ASSERT(glStencilOpSeparate(GL_FRONT, 
                                         getGLStencilOperation(depthStencil.stencilFront.failOp), 
                                         getGLStencilOperation(depthStencil.stencilFront.depthFailOp), 
                                         getGLStencilOperation(depthStencil.stencilFront.passOp)));
            
            // Set back face stencil state
            GL_ASSERT(glStencilFuncSeparate(GL_BACK, 
                                           getGLCompareFunction(depthStencil.stencilBack.compare), 
                                           0, 
                                           depthStencil.stencilReadMask));
            GL_ASSERT(glStencilOpSeparate(GL_BACK, 
                                         getGLStencilOperation(depthStencil.stencilBack.failOp), 
                                         getGLStencilOperation(depthStencil.stencilBack.depthFailOp), 
                                         getGLStencilOperation(depthStencil.stencilBack.passOp)));
        } else {
            GL_ASSERT(glDisable(GL_STENCIL_TEST));
        }
    } else {
        // Disable depth and stencil test
        GL_ASSERT(glDisable(GL_DEPTH_TEST));
        GL_ASSERT(glDisable(GL_STENCIL_TEST));
    }
}

// Apply MultisampleState
void GLPipeline::applyMultisampleState() {
    const MultisampleState& multisample = desc.multisample;
#ifndef GLAD
    // Enable multisampling
    if (multisample.count > 1) {
        GL_ASSERT(glEnable(GL_MULTISAMPLE));
    } else {
        GL_ASSERT(glDisable(GL_MULTISAMPLE));
    }
#endif
    // Set alpha to coverage
    if (multisample.alphaToCoverageEnabled) {
        GL_ASSERT(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    } else {
        GL_ASSERT(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    }
    
    // Note: Sample mask is usually set via glSampleMaski in OpenGL
}


// Apply BlendState
void GLPipeline::applyBlendState() {
    // Enable or disable blending for each color target
    for (size_t i = 0; i < desc.targets.size(); ++i) {
        const ColorTargetState& target = desc.targets[i];

        // Set color write mask
        if (i == 0) { // Only set for the first target in OpenGL
            bool red = (target.writeMask & ColorTargetState::WriteMask_Red) != 0;
            bool green = (target.writeMask & ColorTargetState::WriteMask_Green) != 0;
            bool blue = (target.writeMask & ColorTargetState::WriteMask_Blue) != 0;
            bool alpha = (target.writeMask & ColorTargetState::WriteMask_Alpha) != 0;
            GL_ASSERT(glColorMaski(0, red, green, blue, alpha));
        }

        // Set blend state
        if (target.blend) {
            const BlendState& blend = *target.blend;

            // Enable blending
            GL_ASSERT(glEnable(GL_BLEND));

            // Set blend function for color
            GL_ASSERT(glBlendFuncSeparate(
                getGLBlendFactor(blend.color.srcFactor),
                getGLBlendFactor(blend.color.dstFactor),
                getGLBlendFactor(blend.alpha.srcFactor),
                getGLBlendFactor(blend.alpha.dstFactor)
            ));

            // Set blend equation for color and alpha
            GL_ASSERT(glBlendEquationSeparate(
                getGLBlendOperation(blend.color.operation),
                getGLBlendOperation(blend.alpha.operation)
            ));
        }
        else {
            // Disable blending if no blend state is specified
            GL_ASSERT(glDisable(GL_BLEND));
        }
    }
}