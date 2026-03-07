#ifndef WINDOW_H_
#define WINDOW_H_

//#define VULKAN
//#define WEBGPU

#ifdef VULKAN
#include "vulkan/VKUtil.h"
#endif

struct WGPUChainedStruct;
struct GLFWwindow;


namespace arhi {

class GlfwWindow {
    GLFWwindow* window = nullptr;

public:

    int width = 0;
    int height = 0;
    virtual void onResize(int w, int h) {}

    virtual void onFrame() {}
    virtual void onInit() {}

public:

#ifdef WEBGPU
    WGPUChainedStruct* getSurfaceChain();
#endif // WEBGPU

#ifdef VULKAN
    VkSurfaceKHR createSurface(VkInstance instance);
    std::vector<const char*> getRequiredExtensions();
#endif // VULKAN

public:
    int run(int backend);
};

}
#endif