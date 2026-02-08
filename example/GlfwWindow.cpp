
#include "GLfwWindow.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef __WGVK__
#include <wgvk_structs_impl.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
#  define GLFW_EXPOSE_NATIVE_EMSCRIPTEN
#  ifndef GLFW_PLATFORM_EMSCRIPTEN // not defined in older versions of emscripten
#    define GLFW_PLATFORM_EMSCRIPTEN 0
#  endif
#else // __EMSCRIPTEN__
#  if SUPPORT_XLIB_SURFACE == 1
#    define GLFW_EXPOSE_NATIVE_X11
#  endif
#  if SUPPORT_WAYLAND_SURFACE == 1
#    define GLFW_EXPOSE_NATIVE_WAYLAND
#  endif
#  if SUPPORT_METAL_SURFACE == 1
#    define GLFW_EXPOSE_NATIVE_COCOA
#  endif
#  if SUPPORT_WIN32_SURFACE == 1 || defined(_WIN32) 
#    define GLFW_EXPOSE_NATIVE_WIN32
#  endif
#endif // __EMSCRIPTEN__

#ifdef GLFW_EXPOSE_NATIVE_COCOA
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#endif

#define WIN32_LEAN_AND_MEAN
#define GLEW_STATIC
#include <GL/glew.h>

#ifndef __EMSCRIPTEN__
#  include <GLFW/glfw3native.h>
#endif

using namespace mrhi;

GlfwWindow* g_window = nullptr;


static void resizeCallback(GLFWwindow* window, int width, int height){
    g_window->onResize(width, height);
}

static void CloseWindowCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

#ifdef VULKAN
std::vector<const char*> GlfwWindow::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

VkSurfaceKHR GlfwWindow::createSurface(VkInstance instance)
{
    if (instance == VK_NULL_HANDLE || !window)
    {
        return VK_NULL_HANDLE;
    }

    VkSurfaceKHR surface;


    //VkWin32SurfaceCreateInfoKHR sci = {
    //            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    //            .hinstance = hwndSource->hinstance,
    //            .hwnd = hwndSource->hwnd
    //};
    //vkCreateWin32SurfaceKHR
    //(
    //    instance->instance,
    //    &sci,
    //    NULL,
    //    &ret->surface
    //);

    VkResult errCode = glfwCreateWindowSurface(instance, window, NULL, &surface);

    if (errCode != VK_SUCCESS)
    {
        return nullptr;
    }

    return surface;
}
#endif

int GlfwWindow::run(int backend) {

    g_window = this;

    glfwInit();

    if (backend == 1) {
#if _WIN32
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        glfwWindowHint(GLFW_SAMPLES, 4);
    }
    else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    window = glfwCreateWindow(900, 900, "GLFW Window", NULL, NULL);

    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetKeyCallback(window, CloseWindowCallback);

    glfwGetWindowSize(window, &width, &height);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (backend == 1) {
        //gladLoadGL(glfwGetProcAddress);
        if (GLEW_OK != glewInit())
        {
            printf("Failed to initialize GLEW.\n");
            glfwTerminate();
            return -1;
        }

#ifndef __EMSCRIPTEN__
        glfwSwapInterval(1);
#endif
    }

    onInit();
    
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, ctx, 0, 1);
    #else
    while(!glfwWindowShouldClose(window)){
        onFrame();
        if (backend == 1) {
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    #endif
    return 0;
}

#ifdef WEBGPU

#ifdef _WIN32
WGPUSurfaceSourceWindowsHWND surfaceChainObj;
#elif !defined(__EMSCRIPTEN__)
WGPUSurfaceSourceXlibWindow surfaceChainX11;
#else
WGPUEmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector;
#endif

WGPUChainedStruct* GlfwWindow::getSurfaceChain() {
#ifdef _WIN32
    surfaceChainObj = {
        .chain = {
            .next = NULL,
            .sType = WGPUSType_SurfaceSourceWindowsHWND,
        },
        .hinstance = GetModuleHandle(NULL),
        .hwnd = glfwGetWin32Window(window),
    };
    WGPUChainedStruct* surfaceChain = &surfaceChainObj.chain;
#elif !defined(__EMSCRIPTEN__)
    Display* x11_display = glfwGetX11Display();
    Window x11_window = glfwGetX11Window(window);
    surfaceChainX11.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
    surfaceChainX11.chain.next = NULL;
    surfaceChainX11.display = x11_display;
    surfaceChainX11.window = x11_window;

    struct wl_display* native_display = glfwGetWaylandDisplay();
    struct wl_surface* native_surface = glfwGetWaylandWindow(window);
    WGPUSurfaceSourceWaylandSurface surfaceChainWayland;
    surfaceChainWayland.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
    surfaceChainWayland.chain.next = NULL;
    surfaceChainWayland.display = native_display;
    surfaceChainWayland.surface = native_surface;
    WGPUChainedStruct* surfaceChain = NULL;
    if (x11_window == 0) {
        printf("Using wayland\n");
        surfaceChain = (WGPUChainedStruct*)&surfaceChainWayland;
    }
    else {
        printf("Using X11\n");
        surfaceChain = (WGPUChainedStruct*)&surfaceChainX11;
    }
#else
    fromCanvasHTMLSelector.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;
    fromCanvasHTMLSelector.selector = (WGPUStringView){ "canvas", WGPU_STRLEN };
    WGPUChainedStruct* surfaceChain = (WGPUChainedStruct*)&fromCanvasHTMLSelector;
#endif
    return surfaceChain;
}
#endif

/* ---------- POSIX / Unix-like ---------- */
#if defined(__unix__) || defined(__APPLE__)
#include <time.h>

static inline uint64_t nanoTime(void)
{
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_RAW)        /* Linux, FreeBSD */
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else                                   /* macOS 10.12+, other POSIX */
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

/* ---------- Windows ---------- */
#elif defined(_WIN32)
#include <windows.h>

static inline uint64_t nanoTime(void)
{
    static LARGE_INTEGER freq = { 0 };
    if (freq.QuadPart == 0)               /* one-time init */
        QueryPerformanceFrequency(&freq);

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    /* scale ticks �� ns: (ticks * 1e9) / freq */
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / freq.QuadPart);
}

#else
#error "Platform not supported"
#endif