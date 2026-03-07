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

using namespace arhi;


const char cubeVertexSource[] = R"(
#if __VERSION__ < 450
    #define LAYOUT_LOCATION(locN) 
    #define LAYOUT_BIDING(setN, bindingN) 
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) uniform samplerType samplerN
    #define GET_SAMPLER_CUBE(samplerType, textureN, samplerN) samplerN
#else
    #define LAYOUT_LOCATION(locN) layout(location = locN)
    #define LAYOUT_BIDING(setN, bindingN) layout(set = setN, binding = bindingN)
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN, set = setN, binding = bindingN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) \
        layout(set = setN1, binding = bindingN1) uniform textureType textureN; \
        layout(set = setN1, binding = bindingN1+1) uniform sampler samplerN
    #define GET_SAMPLER_CUBE(samplerType, textureN, samplerN) samplerType(textureN, samplerN)
#endif

LAYOUT_LOCATION(0) in vec3 position;
LAYOUT_LOCATION(1) in vec3 normal;

LAYOUT_LOCATION(0) out vec3 v_normal;
LAYOUT_LOCATION(1) out vec3 v_position;

LAYOUT_BUFFER(0, 3, std140) uniform FrameData {
    vec4 offset;
};

void main() {
    vec3 pos = position * 0.5 + vec3(0.5);
    gl_Position = vec4(pos.x + offset.x - 0.5, pos.y - 0.5, pos.z - 0.5, 1.0);
    v_normal = normal;
    v_position = position;
}
)";


const char cubeFragmentSource[] = R"(
#if __VERSION__ < 450
    #define LAYOUT_LOCATION(locN) 
    #define LAYOUT_BIDING(setN, bindingN) 
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) uniform samplerType samplerN
    #define GET_SAMPLER_CUBE(samplerType, textureN, samplerN) samplerN
#else
    #define LAYOUT_LOCATION(locN) layout(location = locN)
    #define LAYOUT_BIDING(setN, bindingN) layout(set = setN, binding = bindingN)
    #define LAYOUT_BUFFER(setN, bindingN, stdN) layout(stdN, set = setN, binding = bindingN)

    #define DEF_TEXTURE_SAMPLER(setN1, bindingN1, textureType, textureN, samplerType, samplerN) \
        layout(set = setN1, binding = bindingN1) uniform textureType textureN; \
        layout(set = setN1, binding = bindingN1+1) uniform sampler samplerN
    #define GET_SAMPLER_CUBE(samplerType, textureN, samplerN) samplerType(textureN, samplerN)
#endif

LAYOUT_LOCATION(0) in vec3 v_normal;
LAYOUT_LOCATION(1) in vec3 v_position;
LAYOUT_LOCATION(0) out vec4 out_color;

DEF_TEXTURE_SAMPLER(0, 0, textureCube, colDiffuse, samplerCube, grsampler);

void main() {
    vec3 reflectDir = reflect(v_position, normalize(v_normal));
    out_color = texture(GET_SAMPLER_CUBE(samplerCube, colDiffuse, grsampler), reflectDir);
}
)";


struct CubeMesh {
    float t = 0;

    APtr<Pipeline> pipeline;
    //APtr<Texture> texture;
    //APtr<Sampler> sampler;
    APtr<Buffer> indexBuffer;
    APtr<Buffer> vertexBuffer = nullptr;
    APtr<Buffer> uniformBuffer;

    APtr<BindingGroup> bindGroup;

    void render(CommandEncoder* commandEncoder) {
        this->t += 0.001f;
        float FrameData[4] = { t, 0, 0, 0 };
        uniformBuffer->setData(0, FrameData, sizeof(FrameData));
        if (t > 2) {
            t = 0;
        }

        // Record render commands.
        commandEncoder->setPipeline(pipeline.get());
        commandEncoder->setBindingGroup(bindGroup.get(), 0);
        commandEncoder->setIndexBuffer(indexBuffer.get(), 0, IndexFormat::Uint32);
        commandEncoder->setVertexBuffer(vertexBuffer.get(), 0, 0);
        // Test instanced rendering with 10 instances
        commandEncoder->drawIndexed(36, 10, 0, 0, 0); // 6 faces * 2 triangles * 3 indices = 36
    }

    void init() {
        GraphicsDevice* device = GraphicsDevice::cur();
        
        auto vertexShader = device->createShader(ShaderDesc { 
            .source = cubeVertexSource, 
            .stage = ShaderStage::Vertex, 
            .language = ShaderLanguage::GLSL
        });
        auto fragmentShader = device->createShader(ShaderDesc { 
            .source = cubeFragmentSource, 
            .stage = ShaderStage::Fragment, 
            .language = ShaderLanguage::GLSL
        });

        pipeline = device->createPipeline(PipelineDesc {
            .vertexShader = vertexShader.get(),
            .fragmentShader = fragmentShader.get(),
            .bufferLayout = {
                BufferLayout {
                    .stride = sizeof(float) * 6, // 3 for position, 3 for normal
                    .elements = {
                        AttributeElement {.name = "position", .offset = 0,._location = 0,},
                        AttributeElement {.name = "normal", .offset = 3 * sizeof(float),._location = 1,  },
                    },
                }
            },
            .targets = {ColorTargetState {}},
        });

        // Create cubemap texture
        int cubeSize = 32;
        APtr<Texture> texture = device->createTexture(TextureDesc {
            .width = cubeSize, .height = cubeSize,
            .format = TextureFormat::RGBA8Unorm,
            .type = TextureType::Cube, // Use Cube type
            .dimension = 3,
            .usage = TextureDesc::Usage_TextureBinding | TextureDesc::Usage_CopyDst,
            .mipLevelCount = 5, // Enable mipmap with 5 levels
        });

        // Generate cubemap data - 6 faces with different colors
        const int faceColors[6][4] = {
            {255, 0, 0, 255},   // Positive X - Red
            {0, 255, 0, 255},   // Negative X - Green
            {0, 0, 255, 255},   // Positive Y - Blue
            {255, 255, 0, 255}, // Negative Y - Yellow
            {255, 0, 255, 255}, // Positive Z - Magenta
            {0, 255, 255, 255}  // Negative Z - Cyan
        };

        int bytePerPixel = 4;
        size_t faceSize = cubeSize * cubeSize * bytePerPixel;
        size_t totalSize = 6 * faceSize;
        uint8_t* textureData = (uint8_t*)calloc(totalSize, 1);

        // Fill each face with its color
        for (int face = 0; face < 6; face++) {
            uint8_t* faceData = textureData + face * faceSize;
            for (int i = 0; i < cubeSize; i++) {
                for (int j = 0; j < cubeSize; j++) {
                    int p = (i * cubeSize + j) * bytePerPixel;
                    faceData[p] = faceColors[face][0];
                    faceData[p + 1] = faceColors[face][1];
                    faceData[p + 2] = faceColors[face][2];
                    faceData[p + 3] = faceColors[face][3];
                }
            }
        }

        texture->setData(textureData, 0, 6); // Set data for all 6 faces
        free(textureData);

        SamplerDesc samplerDesc;
        samplerDesc.minFilter = SamplerFilterMode::Linear;
        samplerDesc.magFilter = SamplerFilterMode::Linear;
        samplerDesc.mipmapFilter = SamplerFilterMode::Linear;
        samplerDesc.lodMinClamp = 0.0f;
        samplerDesc.lodMaxClamp = 4.0f; // Match mipLevelCount - 1
        APtr<Sampler> sampler = device->createSampler(samplerDesc);
        sampler->texture = arhi::share(texture);

        // Cube indices
        const uint32_t indices[] = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Left face
            8, 9, 10, 8, 10, 11,
            // Right face
            12, 13, 14, 12, 14, 15,
            // Top face
            16, 17, 18, 16, 18, 19,
            // Bottom face
            20, 21, 22, 20, 22, 23
        };
        indexBuffer = device->createBuffer(BufferDesc { 
            .size = sizeof(indices), 
            .usage = BufferDesc::Usage_Index | BufferDesc::Usage_CopyDst });
        indexBuffer->setData(0, indices, sizeof(indices));

        // Cube vertices with normals
        const float scale = 0.5f;
        float vertices[] = {
            // Front face
            -1.0f * scale, -1.0f * scale,  1.0f * scale,  0.0f,  0.0f,  1.0f,
             1.0f * scale, -1.0f * scale,  1.0f * scale,  0.0f,  0.0f,  1.0f,
             1.0f * scale,  1.0f * scale,  1.0f * scale,  0.0f,  0.0f,  1.0f,
            -1.0f * scale,  1.0f * scale,  1.0f * scale,  0.0f,  0.0f,  1.0f,
            // Back face
            -1.0f * scale, -1.0f * scale, -1.0f * scale,  0.0f,  0.0f, -1.0f,
             1.0f * scale, -1.0f * scale, -1.0f * scale,  0.0f,  0.0f, -1.0f,
             1.0f * scale,  1.0f * scale, -1.0f * scale,  0.0f,  0.0f, -1.0f,
            -1.0f * scale,  1.0f * scale, -1.0f * scale,  0.0f,  0.0f, -1.0f,
            // Left face
            -1.0f * scale, -1.0f * scale, -1.0f * scale, -1.0f,  0.0f,  0.0f,
            -1.0f * scale, -1.0f * scale,  1.0f * scale, -1.0f,  0.0f,  0.0f,
            -1.0f * scale,  1.0f * scale,  1.0f * scale, -1.0f,  0.0f,  0.0f,
            -1.0f * scale,  1.0f * scale, -1.0f * scale, -1.0f,  0.0f,  0.0f,
            // Right face
             1.0f * scale, -1.0f * scale, -1.0f * scale,  1.0f,  0.0f,  0.0f,
             1.0f * scale, -1.0f * scale,  1.0f * scale,  1.0f,  0.0f,  0.0f,
             1.0f * scale,  1.0f * scale,  1.0f * scale,  1.0f,  0.0f,  0.0f,
             1.0f * scale,  1.0f * scale, -1.0f * scale,  1.0f,  0.0f,  0.0f,
            // Top face
            -1.0f * scale,  1.0f * scale, -1.0f * scale,  0.0f,  1.0f,  0.0f,
             1.0f * scale,  1.0f * scale, -1.0f * scale,  0.0f,  1.0f,  0.0f,
             1.0f * scale,  1.0f * scale,  1.0f * scale,  0.0f,  1.0f,  0.0f,
            -1.0f * scale,  1.0f * scale,  1.0f * scale,  0.0f,  1.0f,  0.0f,
            // Bottom face
            -1.0f * scale, -1.0f * scale, -1.0f * scale,  0.0f, -1.0f,  0.0f,
             1.0f * scale, -1.0f * scale, -1.0f * scale,  0.0f, -1.0f,  0.0f,
             1.0f * scale, -1.0f * scale,  1.0f * scale,  0.0f, -1.0f,  0.0f,
            -1.0f * scale, -1.0f * scale,  1.0f * scale,  0.0f, -1.0f,  0.0f
        };
        vertexBuffer = device->createBuffer(BufferDesc{
            .size = sizeof(vertices),
            .usage = BufferDesc::Usage_Vertex | BufferDesc::Usage_CopyDst });
        vertexBuffer->setData(0, vertices, sizeof(vertices));

        uniformBuffer = device->createBuffer(BufferDesc{
            .size = 4 * 4,
            .usage = BufferDesc::Usage_Uniform | BufferDesc::Usage_CopyDst });

        BindingGroupDesc bindingGroupDesc;
        bindingGroupDesc.bindingGroup = 0;
        bindingGroupDesc.pipeline = pipeline.get();
        bindingGroupDesc.resources.emplace_back("grsampler", std::move(sampler), 0);
        bindingGroupDesc.resources.emplace_back("colDiffuse", std::move(texture), 0);
        bindingGroupDesc.resources.emplace_back("FrameData", arhi::share(uniformBuffer), 0);
        bindGroup = device->createBindingGroup(std::move(bindingGroupDesc));
    }

    ~CubeMesh() {
        // Smart pointers will automatically manage memory, no need for manual deletion
    }
};


class CubeMapWindow : public GlfwWindow {
    APtr<CubeMesh> mesh = nullptr;
    APtr<Surface> surface = nullptr;

public:
    ~CubeMapWindow() {
        GraphicsDevice::cur()->waitIdle();
#ifdef OPENGL_BACKEND
        delete GraphicsDevice::cur();
#endif
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

        mesh = makeAPtr<CubeMesh>();
        mesh->init();
    }

    void onResize(int w, int h) {
        surface->resize(w, h);
    }

    void onFrame() {
        if (!surface->nextImage()) {
            return;
        }

        APtr<Texture> surfaceTextureView = surface->getCurTextureView();
        CommandEncoder* commandEncoder = surface->getCurCommandEncoder();

        APtr<FrameBuffer> frameBuffer = surface->getCurFrameBuffer();
        if (!frameBuffer) {
            RenderPassDesc renderPassDesc;
            renderPassDesc.colorAttachments.emplace_back();
            auto& colorAttachment = renderPassDesc.colorAttachments.back();
            colorAttachment.view = std::move(surfaceTextureView);
            colorAttachment.loadOp = LoadOp::Clear;
            colorAttachment.storeOp = StoreOp::Store;
            colorAttachment.clearValue[0] = 0.1;
            colorAttachment.clearValue[1] = 0.1;
            colorAttachment.clearValue[2] = 0.1;
            colorAttachment.clearValue[3] = 1.0;
            frameBuffer = GraphicsDevice::cur()->createFrameBuffer(std::move(renderPassDesc));
            surface->cacheFrameBuffer(arhi::share(frameBuffer));
        }

        commandEncoder->beginPass(frameBuffer.get());

        mesh->render(commandEncoder);

        commandEncoder->endPass();

        commandEncoder->submit();

        surface->present();
    }
};

int main2() {
    int rc = -1;
    APtr<CubeMapWindow> window = makeAPtr<CubeMapWindow>();
#ifdef OPENGL_BACKEND
    printf("OpenGL backend - Cubemap Example\n");
    rc = window->run(1);
#elif defined(VULKAN_BACKEND)
    printf("Vulkan backend - Cubemap Example\n");
    rc = window->run(0);
#elif defined(WEBGPU_BACKEND)
    printf("WebGPU backend - Cubemap Example\n");
    rc = window->run(0);
#endif
    return rc;
}
