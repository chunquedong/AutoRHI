
#include "GraphicsDevice.h"

using namespace arhi;

GraphicsDevice* g_graphicsDevice = NULL;

GraphicsDevice* GraphicsDevice::cur() {
	return g_graphicsDevice;
}

APtr<Pipeline> GraphicsDevice::createPipeline(const PipelineDesc& desc) {
    auto it = pipelineCache.find(desc);
    if (it != pipelineCache.end()) {
        return arhi::share(it->second);
    }
    APtr<Pipeline> pipeline = doCreatePipeline(desc);
    pipelineCache[desc] = arhi::share(pipeline);
    return pipeline;
}