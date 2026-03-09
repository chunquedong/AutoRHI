
#include "GraphicsDevice.h"

using namespace arhi;

GraphicsDevice* g_graphicsDevice = nullptr;

GraphicsDevice* GraphicsDevice::cur() {
	return g_graphicsDevice;
}

void GraphicsDevice::destroy() {
    if (g_graphicsDevice) {
        delete g_graphicsDevice;
        g_graphicsDevice = nullptr;
    }
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