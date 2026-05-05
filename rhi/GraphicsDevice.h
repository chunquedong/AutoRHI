/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef GraphicsContext_H_
#define GraphicsContext_H_

#include "Resource.h"
#include "Shader.h"
#include "Pipeline.h"
#include <unordered_map>

namespace arhi {

/**
 * Binding entry
 * Represents a resource binding with a name and offset
 */
struct BindingEntry {
    std::string name;       ///< Name of the binding
    APtr<Resource> resource;     ///< Pointer to the resource
    int offset = 0;         ///< Offset for the binding

    BindingEntry() = default;
    BindingEntry(std::string name, APtr<Resource> resource, int offset = 0)
        : name(std::move(name)), resource(std::move(resource)), offset(offset) {}
    BindingEntry(const BindingEntry&) = delete;
    BindingEntry(BindingEntry&&) noexcept = default;
    BindingEntry& operator=(const BindingEntry&) = delete;
    BindingEntry& operator=(BindingEntry&&) noexcept = default;

    bool operator==(const BindingEntry& other) const {
        return name == other.name &&
               resource == other.resource &&
               offset == other.offset;
    }

    bool operator<(const BindingEntry& other) const {
        if (name != other.name) return name < other.name;
        if (resource != other.resource) return resource < other.resource;
        return offset < other.offset;
    }
};

/**
 * Binding group descriptor
 * Contains all properties needed to create a binding group
 */
struct BindingGroupDesc {
    int bindingGroup = 0;                   ///< Binding group index
    Pipeline* pipeline = nullptr;           ///< Pipeline associated with this binding group
    std::vector<BindingEntry> resources;    ///< List of resources to bind

    BindingGroupDesc() = default;
    BindingGroupDesc(const BindingGroupDesc&) = delete;
    BindingGroupDesc(BindingGroupDesc&&) noexcept = default;
    BindingGroupDesc& operator=(const BindingGroupDesc&) = delete;
    BindingGroupDesc& operator=(BindingGroupDesc&&) noexcept = default;

    bool operator==(const BindingGroupDesc& other) const {
        if (bindingGroup != other.bindingGroup) return false;
        if (pipeline != other.pipeline) return false;
        if (resources.size() != other.resources.size()) return false;
        for (size_t i = 0; i < resources.size(); ++i) {
            const auto& a = resources[i];
            const auto& b = other.resources[i];
            if (a.name != b.name) return false;
            if (a.resource != b.resource) return false;
            if (a.offset != b.offset) return false;
        }
        return true;
    }

    bool operator<(const BindingGroupDesc& other) const {
        if (bindingGroup != other.bindingGroup) return bindingGroup < other.bindingGroup;
        if (pipeline != other.pipeline) return pipeline < other.pipeline;
        if (resources.size() != other.resources.size()) return resources.size() < other.resources.size();
        for (size_t i = 0; i < resources.size(); ++i) {
            const auto& a = resources[i];
            const auto& b = other.resources[i];
            if (a.name != b.name) return a.name < b.name;
            if (a.resource != b.resource) return a.resource < b.resource;
            if (a.offset != b.offset) return a.offset < b.offset;
        }
        return false;
    }
};

/**
 * Binding group interface
 * Represents a collection of resources that can be bound to a pipeline
 */
struct BindingGroup {
protected:
    BindingGroupDesc desc;
public:
    virtual ~BindingGroup() {}
};

/**
 * Load operation enumeration
 * Defines how attachments are loaded at the start of a render pass
 */
enum struct LoadOp {
    Undefined = 0x00000000,            ///< Undefined load operation
    Load = 0x00000001,                  ///< Load existing data from the attachment
    Clear = 0x00000002,                 ///< Clear the attachment to a specified value
    ExpandResolveTexture = 0x00050003,  ///< Expand and resolve texture
};

/**
 * Store operation enumeration
 * Defines how attachments are stored at the end of a render pass
 */
enum struct StoreOp {
    Undefined = 0x00000000,     ///< Undefined store operation
    Store = 0x00000001,          ///< Store the rendered data to the attachment
    Discard = 0x00000002,        ///< Discard the rendered data
};

/**
 * Render pass color attachment
 * Represents a color attachment used in a render pass
 */
struct RenderPassColorAttachment {
    APtr<Texture> view;           ///< Texture view to use as the attachment
    LoadOp loadOp;           ///< Load operation for the attachment
    StoreOp storeOp;         ///< Store operation for the attachment
    float clearValue[4];     ///< Clear value for the attachment (RGBA)

    RenderPassColorAttachment() = default;
    RenderPassColorAttachment(const RenderPassColorAttachment&) = delete;
    RenderPassColorAttachment(RenderPassColorAttachment&&) noexcept = default;
    RenderPassColorAttachment& operator=(const RenderPassColorAttachment&) = delete;
    RenderPassColorAttachment& operator=(RenderPassColorAttachment&&) noexcept = default;

    bool operator==(const RenderPassColorAttachment& other) const {
        if (view != other.view) return false;
        if (loadOp != other.loadOp) return false;
        if (storeOp != other.storeOp) return false;
        for (int i = 0; i < 4; ++i) {
            if (clearValue[i] != other.clearValue[i]) return false;
        }
        return true;
    }

    bool operator<(const RenderPassColorAttachment& other) const {
        if (view != other.view) return view < other.view;
        if (loadOp != other.loadOp) return static_cast<int>(loadOp) < static_cast<int>(other.loadOp);
        if (storeOp != other.storeOp) return static_cast<int>(storeOp) < static_cast<int>(other.storeOp);
        for (int i = 0; i < 4; ++i) {
            if (clearValue[i] != other.clearValue[i]) return clearValue[i] < other.clearValue[i];
        }
        return false;
    }
};

/**
 * Render pass depth-stencil attachment
 * Represents a depth-stencil attachment used in a render pass
 */
struct RenderPassDepthStencilAttachment {
    APtr<Texture> view;               ///< Texture view to use as the attachment
    LoadOp depthLoadOp;          ///< Load operation for the depth component
    StoreOp depthStoreOp;        ///< Store operation for the depth component
    float depthClearValue;       ///< Clear value for the depth component
    uint32_t depthReadOnly;      ///< Whether the depth component is read-only
    LoadOp stencilLoadOp;        ///< Load operation for the stencil component
    StoreOp stencilStoreOp;      ///< Store operation for the stencil component
    uint32_t stencilClearValue;  ///< Clear value for the stencil component
    uint32_t stencilReadOnly;    ///< Whether the stencil component is read-only

    RenderPassDepthStencilAttachment() = default;
    RenderPassDepthStencilAttachment(const RenderPassDepthStencilAttachment&) = delete;
    RenderPassDepthStencilAttachment(RenderPassDepthStencilAttachment&&) noexcept = default;
    RenderPassDepthStencilAttachment& operator=(const RenderPassDepthStencilAttachment&) = delete;
    RenderPassDepthStencilAttachment& operator=(RenderPassDepthStencilAttachment&&) noexcept = default;

    bool operator==(const RenderPassDepthStencilAttachment& other) const {
        if (view != other.view) return false;
        if (depthLoadOp != other.depthLoadOp) return false;
        if (depthStoreOp != other.depthStoreOp) return false;
        if (depthClearValue != other.depthClearValue) return false;
        if (depthReadOnly != other.depthReadOnly) return false;
        if (stencilLoadOp != other.stencilLoadOp) return false;
        if (stencilStoreOp != other.stencilStoreOp) return false;
        if (stencilClearValue != other.stencilClearValue) return false;
        return stencilReadOnly == other.stencilReadOnly;
    }

    bool operator<(const RenderPassDepthStencilAttachment& other) const {
        if (view != other.view) return view < other.view;
        if (depthLoadOp != other.depthLoadOp) return static_cast<int>(depthLoadOp) < static_cast<int>(other.depthLoadOp);
        if (depthStoreOp != other.depthStoreOp) return static_cast<int>(depthStoreOp) < static_cast<int>(other.depthStoreOp);
        if (depthClearValue != other.depthClearValue) return depthClearValue < other.depthClearValue;
        if (depthReadOnly != other.depthReadOnly) return depthReadOnly < other.depthReadOnly;
        if (stencilLoadOp != other.stencilLoadOp) return static_cast<int>(stencilLoadOp) < static_cast<int>(other.stencilLoadOp);
        if (stencilStoreOp != other.stencilStoreOp) return static_cast<int>(stencilStoreOp) < static_cast<int>(other.stencilStoreOp);
        if (stencilClearValue != other.stencilClearValue) return stencilClearValue < other.stencilClearValue;
        return stencilReadOnly < other.stencilReadOnly;
    }
};

/**
 * Render pass descriptor
 * Contains all properties needed to create a render pass
 */
struct RenderPassDesc {
    std::string label;                                             ///< Optional label for the render pass
    std::vector<RenderPassColorAttachment> colorAttachments;       ///< List of color attachments
    RenderPassDepthStencilAttachment* depthStencilAttachment = nullptr; ///< Optional depth-stencil attachment

    RenderPassDesc() = default;
    RenderPassDesc(const RenderPassDesc&) = delete;
    RenderPassDesc(RenderPassDesc&&) noexcept = default;
    RenderPassDesc& operator=(const RenderPassDesc&) = delete;
    RenderPassDesc& operator=(RenderPassDesc&&) noexcept = default;

    bool operator==(const RenderPassDesc& other) const {
        if (label != other.label) return false;
        if (colorAttachments != other.colorAttachments) return false;
        if (depthStencilAttachment != other.depthStencilAttachment) return false;
        return true;
    }

    bool operator<(const RenderPassDesc& other) const {
        if (label != other.label) return (label) < (other.label);
        if (colorAttachments != other.colorAttachments) return colorAttachments < other.colorAttachments;
        return reinterpret_cast<uintptr_t>(depthStencilAttachment) < reinterpret_cast<uintptr_t>(other.depthStencilAttachment);
    }
};

/**
 * Frame buffer interface
 * Represents a frame buffer that can be used in a render pass
 */
struct FrameBuffer {
    virtual ~FrameBuffer() {}
};

/**
 * Command encoder descriptor
 * Contains properties for creating a command encoder
 */
struct CommandEncoderDesc {

};

/**
 * Command encoder interface
 * Represents a command encoder used to record rendering commands
 */
struct CommandEncoder {
    virtual ~CommandEncoder() {}
    
    /**
     * Begin a render pass
     * @param frameBuffer Frame buffer to use for the render pass
     * @return True if the render pass was successfully begun
     */
    virtual bool beginPass(FrameBuffer* frameBuffer) = 0;
    
    /**
     * End the current render pass
     */
    virtual void endPass() = 0;

    /**
     * Set the current pipeline
     * @param pipeline Pipeline to set
     */
    virtual void setPipeline(Pipeline* pipeline) = 0;
    
    /**
     * Set a binding group
     * @param bindingGroup Binding group to set
     * @param groupIndex Index of the binding group
     */
    virtual void setBindingGroup(BindingGroup* bindingGroup, uint32_t groupIndex) = 0;
    
    /**
     * Set the index buffer
     * @param buffer Buffer to use as the index buffer
     * @param offset Offset into the buffer
     * @param indexFormat Format of the indices
     */
    virtual void setIndexBuffer(Buffer* buffer, int offset, IndexFormat indexFormat) = 0;
    
    /**
     * Set a vertex buffer
     * @param buffer Buffer to use as the vertex buffer
     * @param offset Offset into the buffer
     * @param binding Binding index
     */
    virtual void setVertexBuffer(Buffer* buffer, int offset, int binding) = 0;
    
    /**
     * Draw indexed primitives
     * @param indices Number of indices to draw
     * @param instances Number of instances to draw
     * @param firstindex First index to use
     * @param baseVertex Base vertex offset
     * @param firstinstance First instance index
     */
    virtual void drawIndexed(uint32_t indices, uint32_t instances, uint32_t firstindex, int32_t baseVertex, uint32_t firstinstance) = 0;

    virtual void draw(uint32_t vertices, uint32_t instances, uint32_t firstvertex, uint32_t firstinstance) = 0;

    /**
     * Set the scissor rectangle
     * @param x X coordinate of the scissor rectangle
     * @param y Y coordinate of the scissor rectangle
     * @param width Width of the scissor rectangle
     * @param height Height of the scissor rectangle
     */
    virtual void setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    
    /**
     * Set the viewport
     * @param x X coordinate of the viewport
     * @param y Y coordinate of the viewport
     * @param width Width of the viewport
     * @param height Height of the viewport
     * @param minDepth Minimum depth value
     * @param maxDepth Maximum depth value
     */
    virtual void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;

    /**
     * Submit the recorded commands
     */
    virtual void submit() = 0;
};

/**
 * Graphics device interface
 * Represents a graphics device that can create and manage graphics resources
 */
struct GraphicsDevice {
    /**
     * Get the current graphics device
     * @return Pointer to the current graphics device
     */
    static GraphicsDevice* cur();

    static void destroy();
    
    virtual ~GraphicsDevice() {}

    /**
     * Create a surface
     * @param desc Surface descriptor
     * @return Pointer to the created surface
     */
    virtual APtr<Surface> createSurface(const SurfaceDesc& desc) = 0;
    
    /**
     * Create a pipeline
     * @param desc Pipeline descriptor
     * @return Pointer to the created pipeline
     */
    APtr<Pipeline> createPipeline(const PipelineDesc& desc);
    
    /**
     * Do create a pipeline (implementation by subclasses)
     * @param desc Pipeline descriptor
     * @return Pointer to the created pipeline
     */
    virtual APtr<Pipeline> doCreatePipeline(const PipelineDesc& desc) = 0;
    
    /**
     * Create a texture
     * @param desc Texture descriptor
     * @return Pointer to the created texture
     */
    virtual APtr<Texture> createTexture(const TextureDesc& desc) = 0;
    
    /**
     * Create a buffer
     * @param desc Buffer descriptor
     * @return Pointer to the created buffer
     */
    virtual APtr<Buffer> createBuffer(const BufferDesc& desc) = 0;
    
    /**
     * Create a command encoder
     * @param desc Command encoder descriptor
     * @return Pointer to the created command encoder
     */
    virtual APtr<CommandEncoder> createCommandEncoder(const CommandEncoderDesc& desc) = 0;
    
    /**
     * Create a shader
     * @param desc Shader descriptor
     * @return Pointer to the created shader
     */
    virtual APtr<Shader> createShader(const ShaderDesc& desc) = 0;
    
    /**
     * Create a sampler
     * @param desc Sampler descriptor
     * @return Pointer to the created sampler
     */
    virtual APtr<Sampler> createSampler(const SamplerDesc& desc) = 0;
    
    /**
     * Create a binding group
     * @param desc Binding group descriptor
     * @return Pointer to the created binding group
     */
    virtual APtr<BindingGroup> createBindingGroup(BindingGroupDesc&& desc) = 0;
    
    /**
     * Create a frame buffer
     * @param desc Render pass descriptor
     * @return Pointer to the created frame buffer
     */
    virtual APtr<FrameBuffer> createFrameBuffer(RenderPassDesc&& desc) = 0;
    
    /**
     * Wait for the device to become idle
     */
    virtual void waitIdle() {}

protected:
    /**
     * Pipeline cache
     * Stores created pipelines by their descriptors
     */
    std::unordered_map<PipelineDesc, APtr<Pipeline>> pipelineCache;
};

}

namespace std {
    template<>
    struct hash<arhi::BindingGroupDesc> {
        size_t operator()(const arhi::BindingGroupDesc& desc) const {
            size_t h = 0;
            hash_combine(h, desc.bindingGroup);
            hash_combine(h, desc.pipeline);
            for (const auto& entry : desc.resources) {
                hash_combine(h, entry.name);
                hash_combine(h, entry.resource);
                hash_combine(h, entry.offset);
            }
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    template<>
    struct hash<arhi::RenderPassDesc> {
        size_t operator()(const arhi::RenderPassDesc& desc) const {
            size_t h = 0;
            hash_combine(h, (desc.label));
            for (const auto& attachment : desc.colorAttachments) {
                hash_combine(h, attachment.view);
                hash_combine(h, static_cast<int>(attachment.loadOp));
                hash_combine(h, static_cast<int>(attachment.storeOp));
                for (int i = 0; i < 4; ++i) {
                    hash_combine(h, attachment.clearValue[i]);
                }
            }
            hash_combine(h, reinterpret_cast<uintptr_t>(desc.depthStencilAttachment));
            return h;
        }

    private:
        template<typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}

#endif