#include "GLPipeline.h"
#include "GLShader.h"

#include <map>

using namespace mrhi;

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
            avar.format = attribType;
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
        GLint uniformBinding;
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
            GL_ASSERT(glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_BINDING, &uniformBinding));

            uniform.name = uniformName;
            uniform.binding = uniformBinding;
            uniform.type = UniformType::UniformBuffer;
            uniform.bufferBindingType = GL_UNIFORM_BUFFER;
            uniform.bufferMinBindingSize = uniformSize;

            GL_ASSERT(glUniformBlockBinding(program, uniformBlockIndex, uniform.binding));
            reflection.uniforms[uniform.name] = uniform;
        }
    }

    
}

void GLPipeline::buildBindGroupLayoutList(GLDevice* adevice) {

}

GLPipeline* GLPipeline::create(GLDevice* adevice, const PipelineDesc* desc) {
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
        MGP_ERROR("Linking program failed (%s): %s", desc->label, infoLog == NULL ? "" : infoLog);
        delete[](infoLog);

        // Clean up.
        GL_ASSERT(glDeleteProgram(program));

        program = 0;
        return nullptr;
    }

    GLPipeline* pipeline = new GLPipeline();
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
            elem._format = shaderFound->second.format;
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