#ifndef GLPipeline_H_
#define GLPipeline_H_

#include "GLDevice.h"
#include "GLShader.h"

namespace mrhi {

class GLPipeline : public Pipeline {
public:
    PipelineDesc desc;
    ShaderReflection reflection;
    GLuint program;

    static GLPipeline* create(GLDevice* device, const PipelineDesc* d);
    virtual ~GLPipeline();
private:
    void reflect(const PipelineDesc* d);
    void buildBindGroupLayoutList(GLDevice* adevice);
};

}

#endif