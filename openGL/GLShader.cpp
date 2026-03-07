#include "GLShader.h"

using namespace arhi;

APtr<GLShader> GLShader::create(GLDevice* device, const ShaderDesc* d) {
    
    assert(d->source);

    const char* version = NULL;
    if (!version) {
#if defined(OPENGL_ES) || defined(__EMSCRIPTEN__)
        version = "#version 300 es\n";
#else
        version = "#version 330 core\n";
#endif
    }

    const char* defines = "";
    if (d->defines) {
        defines = d->defines;
    }

    GLuint vertexShader;
    GLint length;
    GLint success;

    const GLchar* shaderSource[4] = {
        version, defines, "\n", d->source
    };
    GL_ASSERT(vertexShader = glCreateShader(d->stage == ShaderStage::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER));
    GL_ASSERT(glShaderSource(vertexShader, 4, shaderSource, NULL));
    //GL_ASSERT(glShaderSource(vertexShader, 1, &vshSource, NULL));
    GL_ASSERT(glCompileShader(vertexShader));
    GL_ASSERT(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
    if (success != GL_TRUE)
    {
        GL_ASSERT(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length));
        if (length == 0)
        {
            length = 4096;
        }

        char* infoLog = NULL;
        if (length > 0)
        {
            infoLog = new char[length];
            GL_ASSERT(glGetShaderInfoLog(vertexShader, length, NULL, infoLog));
            infoLog[length - 1] = '\0';
        }

        // Write out the expanded shader file.
        //writeToFile("shader.err", shaderSource[4]);

        // printf(version);
        // printf(defines);
        // printf(vshSource);

        ARHI_ERROR("Compile failed for %s shader '%s' with error '%s'.", (d->stage == ShaderStage::Vertex) ? "Vextex" : "Fragment",
            d->label, infoLog == NULL ? "" : infoLog);
        delete[] infoLog;

        // Clean up.
        GL_ASSERT(glDeleteShader(vertexShader));

        return nullptr;
    }

    auto shader = makeAPtr<GLShader>();
    shader->stage = d->stage;
    shader->shaderModule = vertexShader;
    return shader;
}

GLShader::~GLShader() {
    if (shaderModule && GraphicsDevice::cur()) {
        GL_ASSERT(glDeleteShader(shaderModule));
        shaderModule = 0;
    }
}