#include "VKPipeline.h"
#include "VKShader.h"

#include <map>

using namespace arhi;

// Convert PrimitiveTopology to Vulkan enum
VkPrimitiveTopology getVkPrimitiveTopology(PrimitiveTopology topology) {
    switch (topology) {
    case PrimitiveTopology::PointList:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case PrimitiveTopology::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveTopology::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case PrimitiveTopology::TriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveTopology::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    default:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

// Convert FrontFace to Vulkan enum
VkFrontFace getVkFrontFace(FrontFace frontFace) {
    switch (frontFace) {
    case FrontFace::CCW:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case FrontFace::CW:
        return VK_FRONT_FACE_CLOCKWISE;
    default:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

// Convert CullMode to Vulkan enum
VkCullModeFlags getVkCullMode(CullMode cullMode) {
    switch (cullMode) {
    case CullMode::None:
        return VK_CULL_MODE_NONE;
    case CullMode::Front:
        return VK_CULL_MODE_FRONT_BIT;
    case CullMode::Back:
        return VK_CULL_MODE_BACK_BIT;
    default:
        return VK_CULL_MODE_BACK_BIT;
    }
}

// Convert PolygonMode to Vulkan enum
// VkPolygonMode getVkPolygonMode(PolygonMode polygonMode) {
//     switch (polygonMode) {
//     case PolygonMode::Fill:
//         return VK_POLYGON_MODE_FILL;
//     case PolygonMode::Line:
//         return VK_POLYGON_MODE_LINE;
//     case PolygonMode::Point:
//         return VK_POLYGON_MODE_POINT;
//     default:
//         return VK_POLYGON_MODE_FILL;
//     }
// }

// Convert CompareFunction to Vulkan enum
VkCompareOp getVkCompareFunction(CompareFunction compare) {
    switch (compare) {
    case CompareFunction::Never:
        return VK_COMPARE_OP_NEVER;
    case CompareFunction::Less:
        return VK_COMPARE_OP_LESS;
    case CompareFunction::Equal:
        return VK_COMPARE_OP_EQUAL;
    case CompareFunction::LessEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case CompareFunction::Greater:
        return VK_COMPARE_OP_GREATER;
    case CompareFunction::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case CompareFunction::GreaterEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case CompareFunction::Always:
        return VK_COMPARE_OP_ALWAYS;
    default:
        return VK_COMPARE_OP_LESS;
    }
}

// Convert StencilOperation to Vulkan enum
VkStencilOp getVkStencilOperation(StencilOperation op) {
    switch (op) {
    case StencilOperation::Keep:
        return VK_STENCIL_OP_KEEP;
    case StencilOperation::Zero:
        return VK_STENCIL_OP_ZERO;
    case StencilOperation::Replace:
        return VK_STENCIL_OP_REPLACE;
    case StencilOperation::Invert:
        return VK_STENCIL_OP_INVERT;
    case StencilOperation::IncrementClamp:
        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case StencilOperation::DecrementClamp:
        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case StencilOperation::IncrementWrap:
        return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOperation::DecrementWrap:
        return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    default:
        return VK_STENCIL_OP_KEEP;
    }
}

// Convert SampleCount to Vulkan enum
VkSampleCountFlagBits getVkSampleCount(uint32_t count) {
    switch (count) {
    case 1:
        return VK_SAMPLE_COUNT_1_BIT;
    case 2:
        return VK_SAMPLE_COUNT_2_BIT;
    case 4:
        return VK_SAMPLE_COUNT_4_BIT;
    case 8:
        return VK_SAMPLE_COUNT_8_BIT;
    case 16:
        return VK_SAMPLE_COUNT_16_BIT;
    case 32:
        return VK_SAMPLE_COUNT_32_BIT;
    case 64:
        return VK_SAMPLE_COUNT_64_BIT;
    default:
        return VK_SAMPLE_COUNT_1_BIT;
    }
}


void VKPipeline::buildBindGroupLayoutList(VKDevice* adevice) {

    std::map<int, std::vector<VkDescriptorSetLayoutBinding>> group;
    for (auto it = reflection.uniforms.begin(); it != reflection.uniforms.end(); ++it) {
        const UniformVar& uniform = it->second;
        std::vector<VkDescriptorSetLayoutBinding>& layoutEntries = group[uniform.bindGroup];

        VkDescriptorSetLayoutBinding layoutEntry = {};
        layoutEntry.binding = uniform.binding;
        layoutEntry.descriptorCount = 1;
        layoutEntry.pImmutableSamplers = nullptr;
        layoutEntry.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        //layoutEntry.visibility = uniform.visibility;
        if (uniform.type == UniformType::Texture) {
            layoutEntry.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        }
        else if (uniform.type == UniformType::Sampler) {
            layoutEntry.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        }
        else if (uniform.type == UniformType::UniformBuffer) {
            layoutEntry.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        else {
            ARHI_ERROR("ERROR: unknow uniform type %d\n", uniform.type);
            continue;
        }

        layoutEntries.push_back(layoutEntry);
    }

    //descriptorSetLayouts.clear();
    for (auto it = group.begin(); it != group.end(); ++it) {
        std::vector<VkDescriptorSetLayoutBinding>& layoutEntries = it->second;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(layoutEntries.size());
        layoutInfo.pBindings = layoutEntries.data();

        VkDescriptorSetLayout descriptorSetLayout;
        if (vkCreateDescriptorSetLayout(device->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            ARHI_ERROR("failed to create descriptor set layout!");
        }

        descriptorSetLayouts.push_back(descriptorSetLayout);
    }
}

VkRenderPass VKPipeline::create_render_pass()
{
    //TODO: Get format from desc or device capabilities
    VkFormat swapchain_format = VK_FORMAT_R8G8B8A8_UNORM;

    VkAttachmentDescription attachment{};
    attachment.format = swapchain_format;        // Backbuffer format.
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;                      // Not multisampled.
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                // When starting the frame, we want tiles to be cleared.
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;               // When ending the frame, we want tiles to be written out.
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;            // Don't care about stencil since we're not using it.
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;           // Don't care about stencil since we're not using it.
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                  // The image layout will be undefined when the render pass begins.
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;             // After the render pass is complete, we will transition to PRESENT_SRC_KHR layout.

    // We have one subpass. This subpass has one color attachment.
    // While executing this subpass, the attachment will be in attachment optimal layout.
    VkAttachmentReference color_ref = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

    // We will end up with two transitions.
    // The first one happens right before we start subpass #0, where
    // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
    // The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
    // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_ref;

    // Create a dependency to external events.
    // We need to wait for the WSI semaphore to signal.
    // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
    // actually wait for the semaphore, so we must also wait for that pipeline stage.
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // Since we changed the image layout, we need to make the memory visible to
    // color attachment to modify.
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Finally, create the renderpass.
    VkRenderPassCreateInfo rp_info{};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.attachmentCount = 1;
    rp_info.pAttachments = &attachment;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 1;
    rp_info.pDependencies = &dependency;

    VkRenderPass render_pass;
    VK_CHECK(vkCreateRenderPass(this->device->device, &rp_info, nullptr, &render_pass));
    return render_pass;
}

void VKPipeline::createVertexInputLayout(const PipelineDesc* desc, 
    std::vector<VkVertexInputBindingDescription>& binding_descriptions, 
    std::vector<VkVertexInputAttributeDescription>& attribute_descriptions) {
    int bi = 0;
    for (auto& vertexFormat : desc->bufferLayout) {
        int arrayStride = vertexFormat.stride;
        int begin = attribute_descriptions.size();
        for (int i = 0; i < vertexFormat.elements.size(); ++i) {
            const AttributeElement& bufferInfo = vertexFormat.elements[i];
            VkVertexInputAttributeDescription attr = {};
            if (bufferInfo._location != -1) {
                auto shaderFound = this->reflection.attributes.find(bufferInfo._location);
                if (shaderFound == this->reflection.attributes.end()) {
                    ARHI_ERROR("Not found attribute %d\n", bufferInfo._location);
                    continue;
                }
                attr.format = (VkFormat)shaderFound->second.specificFormat;
                attr.location = bufferInfo._location;
            }
            else {
                auto shaderFound = this->reflection.attributesIndex.find(bufferInfo.name);
                if (shaderFound == this->reflection.attributesIndex.end()) {
                    ARHI_ERROR("Not found attribute %s\n", bufferInfo.name.c_str());
                    continue;
                }
                attr.format = (VkFormat)shaderFound->second.specificFormat;
                attr.location = shaderFound->second.location;
            }
            attr.binding = bi;
            attr.offset = bufferInfo.offset;
            //arrayStride = bufferInfo.stride;
            attribute_descriptions.push_back(attr);
        }

        VkVertexInputBindingDescription binding_description = {};
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        binding_description.binding = bi;
        binding_description.stride = (uint32_t)arrayStride;
        binding_descriptions.push_back(binding_description);
        ++bi;
    }
}

bool VKPipeline::init(VKDevice* adevice, const PipelineDesc* desc) {
    VkDevice device = adevice->device;
    VkQueue queue = adevice->queue;

    VKShader* vertexShader = dynamic_cast<VKShader*>(desc->vertexShader);
    VKShader* fragmentShader = nullptr;
    if (desc->fragmentShader) {
        fragmentShader = dynamic_cast<VKShader*>(desc->fragmentShader);
    }

    this->device = adevice;
    //reflect
    reflect(desc);

    buildBindGroupLayoutList(adevice);

    // Create a blank pipeline layout.
    // We are not binding any resources to the pipeline in this first sample.
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

    // Define the vertex input binding.
    std::vector<VkVertexInputBindingDescription> binding_descriptions;
    // Define the vertex input attribute.
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
    createVertexInputLayout(desc, binding_descriptions, attribute_descriptions);

    // The Vertex input properties define the interface between the vertex buffer and the vertex shader.

    // Specify the primitive topology from PrimitiveState.
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = getVkPrimitiveTopology(desc->primitive.topology);

    // Define the pipeline vertex input.
    VkPipelineVertexInputStateCreateInfo vertex_input{};
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input.vertexBindingDescriptionCount = (uint32_t)binding_descriptions.size();
    vertex_input.pVertexBindingDescriptions = binding_descriptions.data();
    vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input.pVertexAttributeDescriptions = attribute_descriptions.data();

    // Specify rasterization state from PrimitiveState.
    VkPipelineRasterizationStateCreateInfo raster{};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //raster.polygonMode = getVkPolygonMode(desc->primitive.polygonMode);
    raster.cullMode = getVkCullMode(desc->primitive.cullMode);
    raster.frontFace = getVkFrontFace(desc->primitive.frontFace);
    raster.lineWidth = 1.0f;

    if (desc->depthStencil.depthBiasEnabled) {
        raster.depthBiasEnable = true;
        raster.depthBiasConstantFactor = desc->depthStencil.depthBias;
        raster.depthBiasSlopeFactor = desc->depthStencil.depthBiasSlopeScale;
        raster.depthBiasClamp = desc->depthStencil.depthBiasClamp;
    }
    else {
        raster.depthBiasEnable = false;
        raster.depthBiasConstantFactor = 0;
        raster.depthBiasSlopeFactor = 0;
        raster.depthBiasClamp = 0;
    }
    // Our attachment will write to all color channels, but no blending is enabled.
    VkPipelineColorBlendAttachmentState blend_attachment{};
    blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blend{};
    blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.attachmentCount = 1;
    blend.pAttachments = &blend_attachment;

    // We will have one viewport and scissor box.
    VkPipelineViewportStateCreateInfo viewport{};
    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;

    // Set up depth stencil state from DepthStencilState.
    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    if (desc->depthStencil.depthTestEnabled) {
        depth_stencil.depthTestEnable = true;
        depth_stencil.depthWriteEnable = desc->depthStencil.depthWriteEnabled;
        depth_stencil.depthCompareOp = getVkCompareFunction(desc->depthStencil.depthCompare);
    }
    else {
        depth_stencil.depthTestEnable = false;
        depth_stencil.depthWriteEnable = false;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    }

    if (desc->depthStencil.stencilTestEnabled) {
        depth_stencil.stencilTestEnable = true;
        depth_stencil.front.failOp = getVkStencilOperation(desc->depthStencil.stencilFront.failOp);
        depth_stencil.front.passOp = getVkStencilOperation(desc->depthStencil.stencilFront.passOp);
        depth_stencil.front.depthFailOp = getVkStencilOperation(desc->depthStencil.stencilFront.depthFailOp);
        depth_stencil.front.compareOp = getVkCompareFunction(desc->depthStencil.stencilFront.compare);
        depth_stencil.back.failOp = getVkStencilOperation(desc->depthStencil.stencilBack.failOp);
        depth_stencil.back.passOp = getVkStencilOperation(desc->depthStencil.stencilBack.passOp);
        depth_stencil.back.depthFailOp = getVkStencilOperation(desc->depthStencil.stencilBack.depthFailOp);
        depth_stencil.back.compareOp = getVkCompareFunction(desc->depthStencil.stencilBack.compare);
    }
    else {
        depth_stencil.stencilTestEnable = false;
        depth_stencil.front.failOp = VK_STENCIL_OP_KEEP;
        depth_stencil.front.passOp = VK_STENCIL_OP_KEEP;
        depth_stencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
        depth_stencil.front.compareOp = VK_COMPARE_OP_ALWAYS;
        depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
        depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
        depth_stencil.back.depthFailOp = VK_STENCIL_OP_KEEP;
        depth_stencil.back.compareOp =  VK_COMPARE_OP_ALWAYS;
    }
    depth_stencil.maxDepthBounds = 1.0f;
    depth_stencil.front.compareMask = desc->depthStencil.stencilReadMask;
    depth_stencil.front.writeMask = desc->depthStencil.stencilWriteMask;
    depth_stencil.back.compareMask = desc->depthStencil.stencilReadMask;
    depth_stencil.back.writeMask = desc->depthStencil.stencilWriteMask;

    // Set up multisample state from MultisampleState.
    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = getVkSampleCount(desc->multisample.count);
    multisample.sampleShadingEnable = desc->multisample.count > 1;
    multisample.minSampleShading = 1.0f;
    multisample.alphaToCoverageEnable = desc->multisample.alphaToCoverageEnabled;
    multisample.alphaToOneEnable = false;
    multisample.pSampleMask = &desc->multisample.mask;

    // Specify that these states will be dynamic, i.e. not part of pipeline state object.
    VkDynamicState dynamics[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamic{};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = 2;
    dynamic.pDynamicStates = dynamics;

    
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

    // Vertex stage of the pipeline
    VkPipelineShaderStageCreateInfo vertexStage{};
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.module = vertexShader->shaderModule;
    vertexStage.pName = vertexShader->entryPoint;
    shader_stages.emplace_back(vertexStage);

    // Fragment stage of the pipeline
    VkPipelineShaderStageCreateInfo fragmentStage{};
    fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.module = fragmentShader->shaderModule;
    fragmentStage.pName = fragmentShader->entryPoint;
    shader_stages.emplace_back(fragmentStage);


    this->renderPass = create_render_pass();

    VkGraphicsPipelineCreateInfo pipe{};
    pipe.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipe.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipe.pStages = shader_stages.data();
    pipe.pVertexInputState = &vertex_input;
    pipe.pInputAssemblyState = &input_assembly;
    pipe.pViewportState = &viewport;
    pipe.pRasterizationState = &raster;
    pipe.pMultisampleState = &multisample;
    pipe.pDepthStencilState = &depth_stencil;
    pipe.pColorBlendState = &blend;
    pipe.pDynamicState = &dynamic;
    pipe.layout = pipelineLayout;        // We need to specify the pipeline layout up front
    pipe.renderPass = this->renderPass;             // We need to specify the render pass up front

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, &pipeline));

    // Don't destroy the render pass because the pipeline will reference it
    // vkDestroyRenderPass(device, renderPass, nullptr);

    return true;
}

VKPipeline::~VKPipeline() {
    if (pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device->device, pipeline, nullptr);
    }

    if (renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(device->device, renderPass, nullptr);
    }

    for (auto& dsl : descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(device->device, dsl, nullptr);
    }

    if (pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device->device, pipelineLayout, nullptr);
    }
}