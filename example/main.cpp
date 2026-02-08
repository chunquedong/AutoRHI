#include <stdlib.h>

#define OPENGL_BACKEND
//#define VULKAN_BACKEND
//#define WEBGPU_BACKEND

#include "GlfwWindow.h"
#ifdef OPENGL_BACKEND
#include "openGL/GLDevice.h"
#elif defined(VULKAN_BACKEND)
#include "vulkan/VKDevice.h"
#elif defined(WEBGPU_BACKEND)
#include "webgpu/WGDevice.h"
#else
#include "rhi/GraphicsDevice.h"
#endif

using namespace mrhi;


const char vertexSource[] = R"(
#if __VERSION__ < 450
    #define LAYOUT_LOCATION(locN)
    #define LAYOUT_BIDING(setN, bindingN) 
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) uniform samplerType samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerN
#else
    #define LAYOUT_LOCATION(locN) layout(location = locN)
    #define LAYOUT_BIDING(setN, bindingN) layout(set = setN, binding = bindingN)
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN, set = setN, binding = bindingN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) \
        layout(set = setN1, binding = bindingN1) uniform textureType textureN; \
        layout(set = setN1, binding = bindingN1+1) uniform sampler samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerType(textureN, samplerN)
#endif

LAYOUT_LOCATION(0) in vec2 position;
LAYOUT_LOCATION(1) in vec2 uv;

LAYOUT_LOCATION(0) out vec2 v_uv;

LAYOUT_BUFFER(0, 3, std140) uniform FrameData {
    vec4 offset;
};

void main() {
    gl_Position = vec4(position.x+offset.x, position.y, 0.0, 1.0);
    v_uv = uv;
}
)";


const char fragmentSource[] = R"(
#if __VERSION__ < 450
    #define LAYOUT_LOCATION(locN)
    #define LAYOUT_BIDING(setN, bindingN) 
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) uniform samplerType samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerN
#else
    #define LAYOUT_LOCATION(locN) layout(location = locN)
    #define LAYOUT_BIDING(setN, bindingN) layout(set = setN, binding = bindingN)
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN, set = setN, binding = bindingN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) \
        layout(set = setN1, binding = bindingN1) uniform textureType textureN; \
        layout(set = setN1, binding = bindingN1+1) uniform sampler samplerN
    #define GET_SAMPLER_2D(samplerType, textureN, samplerN) samplerType(textureN, samplerN)
#endif

LAYOUT_LOCATION(0) in vec2 v_uv;
LAYOUT_LOCATION(0) out vec4 out_color;

DEF_TEXTURE_SAMPLER(0, 0, texture2D, colDiffuse, sampler2D, grsampler);

void main() {
    out_color = texture(GET_SAMPLER_2D(sampler2D, colDiffuse, grsampler), v_uv);
}
)";


struct Mesh {
    float t = 0;

    Pipeline* pipeline;
    Texture* texture;
    Sampler* sampler;
    Buffer* indexBuffer;
    Buffer* vertexBuffer = nullptr;
    Buffer* uniformBuffer;

    BindingGroup* bindGroup;

    void render(CommandEncoder* commandEncoder) {
        this->t += 0.001f;
        float FrameData[4] = { t, 0, 0, 0 };
        uniformBuffer->setData(0, FrameData, sizeof(FrameData));
        if (t > 2) {
            t = 0;
        }

        // Record render commands.
        commandEncoder->setPipeline(pipeline);
        commandEncoder->setBindingGroup(bindGroup, 0);
        commandEncoder->setIndexBuffer(indexBuffer, 0, IndexFormat::Uint32);
        commandEncoder->setVertexBuffer(vertexBuffer, 0, 0);
        commandEncoder->drawIndexed(6, 1, 0, 0, 0);
    }

    void init() {
        GraphicsDevice* device = GraphicsDevice::cur();
        
        Shader* vertexShader = device->createShader(ShaderDesc { 
            .source = vertexSource, 
            .stage = ShaderStage::Vertex, 
            .language = ShaderLanguage::GLSL
        });
        Shader* fragmentShader = device->createShader(ShaderDesc { 
            .source = fragmentSource, 
            .stage = ShaderStage::Fragment, 
            .language = ShaderLanguage::GLSL
        });

        pipeline = device->createPipeline(PipelineDesc {
            .vertexShader = vertexShader,
            .fragmentShader = fragmentShader,
            .bufferLayout = {
                BufferLayout {
                    .stride = sizeof(float) * 4,
                    .elements = {
                        AttributeElement {.name = "position", .offset = 0,._location = 0,},
                        AttributeElement {.name = "uv", .offset = 2 * sizeof(float),._location = 1,  },
                    },
                }
            },
            .targets = {ColorTargetState {}},
        });
        delete vertexShader;
        delete fragmentShader;

        int w = 10;
        int h = 10;
        int bytePerPixel = 4;
        texture = device->createTexture(TextureDesc {
            .width = w, .height = h,
            .format = TextureFormat::RGBA8Unorm,
            .type = TextureType::_2D,
            .dimension = 2,
            .usage = TextureDesc::Usage_TextureBinding | TextureDesc::Usage_CopyDst,
            .bytePerPixel = 4,
        });

        uint8_t* textureData = (uint8_t*)calloc(w * h, bytePerPixel);
        for (size_t i = 0; i < h; i++) {
            for (size_t j = 0; j < w; j++) {
                int p = (i * w + j) * bytePerPixel;
                if (i == j) {
                    textureData[p] = 255;
                    textureData[p + 1] = 0;
                    textureData[p + 2] = 0;
                    textureData[p + 3] = 255;
                }
                else {
                    textureData[p] = 50;
                    textureData[p + 1] = 100;
                    textureData[p + 2] = 255;
                    textureData[p + 3] = 255;
                }
            }
        }
        texture->setData(textureData, 0, 0);
        free(textureData);

        sampler = device->createSampler(SamplerDesc{});
        sampler->texture = texture;

        const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };
        indexBuffer = device->createBuffer(BufferDesc { 
            .size = sizeof(indices), 
            .usage = BufferDesc::Usage_Index | BufferDesc::Usage_CopyDst });
        indexBuffer->setData(0, indices, sizeof(indices));


        const float scale = 0.6f;
        float adhocVertices[16] = {
            0.0f * scale, 0.0f * scale, 0.0f, 0.0f,
            1.0f * scale, 0.0f * scale, 1.0f, 0.0f,
            1.0f * scale, 1.0f * scale, 1.0f, 1.0f,
            0.0f * scale, 1.0f * scale, 0.0f, 1.0f,
        };
        vertexBuffer = device->createBuffer(BufferDesc{
            .size = sizeof(adhocVertices),
            .usage = BufferDesc::Usage_Vertex | BufferDesc::Usage_CopyDst });
        vertexBuffer->setData(0, adhocVertices, sizeof(adhocVertices));


        uniformBuffer = device->createBuffer(BufferDesc{
            .size = 4 * 4,
            .usage = BufferDesc::Usage_Uniform | BufferDesc::Usage_CopyDst });

        bindGroup = device->createBindingGroup(BindingGroupDesc{
            .bindingGroup = 0,
            .pipeline = pipeline,
            .resources = {
                {"grsampler", sampler},
                {"colDiffuse", texture},
                {"FrameData", uniformBuffer},
            },
        });
    }

    ~Mesh() {
        delete bindGroup;
        delete pipeline;
        delete texture;
        delete indexBuffer;
        delete vertexBuffer;
        delete uniformBuffer;
        delete sampler;
    }
};


class MainWindow : public GlfwWindow {
    Mesh* mesh = nullptr;
    Surface* surface = nullptr;

public:
    ~MainWindow() {
        GraphicsDevice::cur()->waitIdle();
#ifdef OPENGL_BACKEND
        delete GraphicsDevice::cur();
#endif
        delete mesh;
        delete surface;
#ifndef OPENGL_BACKEND
        delete GraphicsDevice::cur();
#endif
    }

    void onInit() {
#ifdef OPENGL_BACKEND
        GLDevice* device = new GLDevice();
        device->init();
        surface = device->createSurface(SurfaceDesc{ .width = width, .height = height, });
#elif defined(VULKAN_BACKEND)
        VKDevice* device = new VKDevice();
        bool isDebug = true;
        std::vector<const char*> extension = getRequiredExtensions();
        auto instance = device->initInstance(extension, isDebug);
        auto vkSurface = createSurface(instance);
        surface = device->createSurface(SurfaceDesc{ .width = width, .height = height, .surfaceChain = vkSurface, });
#elif defined(WEBGPU_BACKEND)
        WGDevice* device = new WGDevice();
        device->init();
        surface = device->createSurface(SurfaceDesc{ .width = width, .height = height, .surfaceChain = getSurfaceChain() });
#endif

        mesh = new Mesh();
        mesh->init();
    }

    void onResize(int w, int h) {
        surface->resize(w, h);
    }

    void onFrame() {
        if (!surface->nextImage()) {
            return;
        }

        Texture* surfaceTextureView = surface->getCurTextureView();
        CommandEncoder* commandEncoder = surface->getCurCommandEncoder();

        FrameBuffer* frameBuffer = surface->getCurFrameBuffer();
        if (!frameBuffer) {
            frameBuffer = GraphicsDevice::cur()->createFrameBuffer(RenderPassDesc{
                .colorAttachments = {
                    RenderPassColorAttachment {
                        .view = surfaceTextureView,
                        .loadOp = LoadOp::Clear,
                        .storeOp = StoreOp::Store,
                        .clearValue = { 0.5, 0.0, 0, 1.0 },
                    },
                },
            });
            surface->cacheFrameBuffer(frameBuffer);
        }

        commandEncoder->beginPass(frameBuffer);

        mesh->render(commandEncoder);

        commandEncoder->endPass();

        commandEncoder->submit();

        surface->present();
    }
};

int main() {
    int rc = -1;
    MainWindow* window = new MainWindow();
#ifdef OPENGL_BACKEND
    printf("OpenGL backend\n");
    rc = window->run(1);
#elif defined(VULKAN_BACKEND)
    printf("Vulkan backend\n");
    rc = window->run(0);
#elif defined(WEBGPU_BACKEND)
    printf("WebGPU backend\n");
    rc = window->run(0);
#endif
    delete window;
    return rc;
}

