#ifndef WGPipeline_H_
#define WGPipeline_H_

#include "WGDevice.h"
#include "WGShader.h"

namespace mrhi {

class WGPipeline : public Pipeline {
public:

    WGPURenderPipeline pipeline = nullptr;
    std::vector<WGPUBindGroupLayout> bindGroupLayoutList;

    bool init(WGDevice* device, const PipelineDesc* d);
    virtual ~WGPipeline();
private:

    void buildBindGroupLayoutList(WGDevice* adevice);
    void createVertexInputLayout(const PipelineDesc* desc,
        std::vector<WGPUVertexBufferLayout>& vertexBufferLayoutList,
        std::vector<WGPUVertexAttribute>& vertexAttributes);
};

}

#endif