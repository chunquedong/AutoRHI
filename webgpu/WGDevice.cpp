#include "WGDevice.h"
#include "WGCommandEncoder.h"

using namespace mrhi;

extern GraphicsDevice* g_graphicsDevice;

static void adapterCallbackFunction(
    WGPURequestAdapterStatus status,
    WGPUAdapter adapter,
    WGPUStringView label,
    void* userdata1,
    void* userdata2
) {
    *((WGPUAdapter*)userdata1) = adapter;
}
static void deviceCallbackFunction(
    WGPURequestDeviceStatus status,
    WGPUDevice device,
    WGPUStringView message,
    void* userdata1,
    void* userdata2
) {
    *((WGPUDevice*)userdata1) = device;
}

#define TimeoutNS 1000000000

void WGDevice::init() {
    //#ifdef __EMSCRIPTEN__
    //WGPUInstance instance = wgpuCreateInstance(NULL);
    //#else
#ifndef __EMSCRIPTEN__
    WGPUInstanceLayerSelection lsel = {
        .chain = {
            .next = NULL,
            .sType = WGPUSType_InstanceLayerSelection
        }
    };
    const char* layernames[] = { "VK_LAYER_KHRONOS_validation" };
    lsel.instanceLayers = layernames;
    lsel.instanceLayerCount = 1;
    WGPUInstanceFeatureName instanceFeatures[2] = {
        WGPUInstanceFeatureName_TimedWaitAny,
        WGPUInstanceFeatureName_ShaderSourceSPIRV,
    };
    WGPUInstanceDescriptor instanceDescriptor = {
        .nextInChain =
        #ifdef NDEBUG
        NULL
        #else
        & lsel.chain
        #endif
        ,
        .requiredFeatureCount = 2,
        .requiredFeatures = instanceFeatures,
    };
#else
    WGPUInstanceDescriptor instanceDescriptor = {
        .capabilities = {
            .timedWaitAnyEnable = 1
        }
    };
#endif
    WGPUInstance instance = wgpuCreateInstance(&instanceDescriptor);

    //#endif

    WGPURequestAdapterOptions adapterOptions = { 0 };
    adapterOptions.featureLevel = WGPUFeatureLevel_Compatibility;
    adapterOptions.backendType = WGPUBackendType_WebGPU;
    WGPURequestAdapterCallbackInfo adapterCallback = { 0 };
    adapterCallback.callback = adapterCallbackFunction;
    adapterCallback.mode = WGPUCallbackMode_WaitAnyOnly;

    WGPUAdapter requestedAdapter;
    adapterCallback.userdata1 = (void*)&requestedAdapter;


    WGPUFuture aFuture = wgpuInstanceRequestAdapter(instance, &adapterOptions, adapterCallback);
    WGPUFutureWaitInfo winfo = {
        .future = aFuture,
        .completed = 0
    };

    wgpuInstanceWaitAny(instance, 1, &winfo, TimeoutNS);
    WGPUStringView deviceLabel = { "WGPU Device", sizeof("WGPU Device") - 1 };
    if (requestedAdapter == NULL) {
        MGP_ERROR("Adapter is NULL\n");
    }

    WGPUDeviceDescriptor deviceDescriptor = {
        .nextInChain = 0,
        .label = deviceLabel,
        .requiredFeatureCount = 0,
        .requiredFeatures = NULL,
        .requiredLimits = NULL,
        .defaultQueue = {0},
        .deviceLostCallbackInfo = {0},
        .uncapturedErrorCallbackInfo = {0},
    };

    WGPUDevice device = NULL;
    WGPURequestDeviceCallbackInfo requestDeviceCallbackInfo = {
        .mode = WGPUCallbackMode_WaitAnyOnly,
        .callback = deviceCallbackFunction,
        .userdata1 = &device
    };
    WGPUFuture requestDeviceFuture = wgpuAdapterRequestDevice(requestedAdapter, &deviceDescriptor, requestDeviceCallbackInfo);
    WGPUFutureWaitInfo requestDeviceFutureWaitInfo = {
        .future = requestDeviceFuture,
        .completed = 0
    };
    wgpuInstanceWaitAny(instance, 1, &requestDeviceFutureWaitInfo, TimeoutNS);

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    
    this->instance = instance;
    this->adapter = requestedAdapter;
    this->device = device;
    //this->surface = surface;
    this->queue = queue;
    //this->window = window;
    g_graphicsDevice = this;
}

WGDevice::~WGDevice() {

    if (g_graphicsDevice == this) {
        g_graphicsDevice = nullptr;
    }

    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuInstanceRelease(instance);
}


Surface* WGDevice::createSurface(const SurfaceDesc& d) {
    WGSurface* fbo = new WGSurface();
    fbo->init(this, &d);
    return fbo;
}

Pipeline* WGDevice::createPipeline(const PipelineDesc& d) {
    WGPipeline* t = new WGPipeline();
    t->init(this, &d);
    return t;
}

Texture* WGDevice::createTexture(const TextureDesc& d) {
    WGTexture* t = new WGTexture();
    t->init(this, &d);
    return t;
}

Buffer* WGDevice::createBuffer(const BufferDesc& d) {
    WGBuffer* t = new WGBuffer();
    t->init(this, &d);
    return t;
}

CommandEncoder* WGDevice::createCommandEncoder(const CommandEncoderDesc& d) {
    WGCommandEncoder* t = new WGCommandEncoder();
    t->init(this, &d);
    return t;
}

Shader* WGDevice::createShader(const ShaderDesc& d) {
    return WGShader::create(this, &d);
}

Sampler* WGDevice::createSampler(const SamplerDesc& d) {
    WGSampler* t = new WGSampler();
    t->init(this, &d);
    return t;
}

BindingGroup* WGDevice::createBindingGroup(const BindingGroupDesc& d) {
    WGBindingGroup* t = new WGBindingGroup();
    t->init(this, &d);
    return t;
}

FrameBuffer* WGDevice::createFrameBuffer(const RenderPassDesc& desc)
{
    WGFrameBuffer* t = new WGFrameBuffer();
    t->desc = desc;
    return t;
}