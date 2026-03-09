#ifndef WINDOW_H_
#define WINDOW_H_


#ifdef USE_VULKAN
#include "vulkan/VKUtil.h"
#endif

#include "rhi/GraphicsDevice.h"

struct WGPUChainedStruct;
struct GLFWwindow;

namespace arhi {

enum struct Backend {
    OpenGL, Vulkan, WebGPU
};

class GlfwWindow {
    GLFWwindow* window = nullptr;
protected:
    Backend backend;
    APtr<Surface> surface = nullptr;
public:
    virtual ~GlfwWindow();

    int width = 0;
    int height = 0;
    virtual void onResize(int w, int h);

    virtual void onFrame() {}
    virtual void onInit() {}
    virtual void onDestroy() {}
public:
    int run(Backend backend);

public:
#ifdef USE_WEBGPU
    WGPUChainedStruct* getSurfaceChain();
#endif // USE_WEBGPU

#ifdef USE_VULKAN
    VkSurfaceKHR createVkSurface(VkInstance instance);
    std::vector<const char*> getRequiredExtensions();
#endif // USE_VULKAN

    APtr<Surface> initDeviceSurface(Backend backend);
};

}
#endif