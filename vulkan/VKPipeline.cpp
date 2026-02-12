#include "VKPipeline.h"
#include "VKShader.h"

#include <map>

using namespace arhi;


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
            MGP_ERROR("ERROR: unknow uniform type %d\n", uniform.type);
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
            MGP_ERROR("failed to create descriptor set layout!");
        }

        descriptorSetLayouts.push_back(descriptorSetLayout);
    }
}

VkRenderPass VKPipeline::create_render_pass()
{
    //TODO
    VkFormat swapchain_format = VK_FORMAT_R8G8B8A8_UNORM;

    VkAttachmentDescription attachment{
        .format = swapchain_format,        // Backbuffer format.
        .samples = VK_SAMPLE_COUNT_1_BIT,                      // Not multisampled.
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,                // When starting the frame, we want tiles to be cleared.
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,               // When ending the frame, we want tiles to be written out.
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,            // Don't care about stencil since we're not using it.
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,           // Don't care about stencil since we're not using it.
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,                  // The image layout will be undefined when the render pass begins.
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR             // After the render pass is complete, we will transition to PRESENT_SRC_KHR layout.
    };

    // We have one subpass. This subpass has one color attachment.
    // While executing this subpass, the attachment will be in attachment optimal layout.
    VkAttachmentReference color_ref = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

    // We will end up with two transitions.
    // The first one happens right before we start subpass #0, where
    // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
    // The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
    // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
    VkSubpassDescription subpass{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_ref,
    };

    // Create a dependency to external events.
    // We need to wait for the WSI semaphore to signal.
    // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
    // actually wait for the semaphore, so we must also wait for that pipeline stage.
    VkSubpassDependency dependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    // Since we changed the image layout, we need to make the memory visible to
    // color attachment to modify.
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Finally, create the renderpass.
    VkRenderPassCreateInfo rp_info{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency };

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
                    MGP_ERROR("Not found attribute %d\n", bufferInfo._location);
                    continue;
                }
                attr.format = (VkFormat)shaderFound->second.format;
                attr.location = bufferInfo._location;
            }
            else {
                auto shaderFound = this->reflection.attributesIndex.find(bufferInfo.name);
                if (shaderFound == this->reflection.attributesIndex.end()) {
                    MGP_ERROR("Not found attribute %s\n", bufferInfo.name.c_str());
                    continue;
                }
                attr.format = (VkFormat)shaderFound->second.format;
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

    // Specify we will use triangle lists to draw geometry.
    VkPipelineInputAssemblyStateCreateInfo input_assembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };

    // Define the pipeline vertex input.
    VkPipelineVertexInputStateCreateInfo vertex_input{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = (uint32_t)binding_descriptions.size(),
        .pVertexBindingDescriptions = binding_descriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
        .pVertexAttributeDescriptions = attribute_descriptions.data() };

    // Specify rasterization state.
    VkPipelineRasterizationStateCreateInfo raster{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth = 1.0f };

    // Our attachment will write to all color channels, but no blending is enabled.
    VkPipelineColorBlendAttachmentState blend_attachment{
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };

    VkPipelineColorBlendStateCreateInfo blend{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blend_attachment };

    // We will have one viewport and scissor box.
    VkPipelineViewportStateCreateInfo viewport{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1 };

    // Disable all depth testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

    // No multisampling.
    VkPipelineMultisampleStateCreateInfo multisample{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT };

    // Specify that these states will be dynamic, i.e. not part of pipeline state object.
    VkDynamicState dynamics[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamic{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamics };

    
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

    // Vertex stage of the pipeline
    shader_stages.emplace_back(VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertexShader->shaderModule,
        .pName = vertexShader->entryPoint });

    // Fragment stage of the pipeline
    shader_stages.emplace_back(VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragmentShader->shaderModule,
        .pName = fragmentShader->entryPoint });


    VkRenderPass renderPass = create_render_pass();

    VkGraphicsPipelineCreateInfo pipe{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(shader_stages.size()),
        .pStages = shader_stages.data(),
        .pVertexInputState = &vertex_input,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport,
        .pRasterizationState = &raster,
        .pMultisampleState = &multisample,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &blend,
        .pDynamicState = &dynamic,
        .layout = pipelineLayout,        // We need to specify the pipeline layout up front
        .renderPass = renderPass             // We need to specify the render pass up front
    };

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, &pipeline));

    vkDestroyRenderPass(device, renderPass, nullptr);

    return true;
}

VKPipeline::~VKPipeline() {
    if (pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device->device, pipeline, nullptr);
    }

    for (auto& dsl : descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(device->device, dsl, nullptr);
    }

    if (pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device->device, pipelineLayout, nullptr);
    }
}