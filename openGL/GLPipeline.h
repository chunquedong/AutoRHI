/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef GLPipeline_H_
#define GLPipeline_H_

#include "GLDevice.h"
#include "GLShader.h"

namespace arhi {

class GLPipeline : public Pipeline {
public:
    PipelineDesc desc;
    ShaderReflection reflection;
    GLuint program;

    static APtr<GLPipeline> create(GLDevice* device, const PipelineDesc* d);
    virtual ~GLPipeline();
    
    void applyState();
private:
    // State application methods
    void applyPrimitiveState();
    void applyDepthStencilState();
    void applyBlendState();
    void applyMultisampleState();
private:
    void reflect(const PipelineDesc* d);
    void buildBindGroupLayoutList(GLDevice* adevice);
};

}

#endif