/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef VKPipeline_H_
#define VKPipeline_H_

#include "VKDevice.h"
#include "VKShader.h"

namespace arhi {

class VKPipeline : public Pipeline {
	VKDevice* device;
public:

	/// The graphics pipeline.
	VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    bool init(VKDevice* device, const PipelineDesc* d);
    virtual ~VKPipeline();
private:
    void buildBindGroupLayoutList(VKDevice* adevice);
	VkRenderPass create_render_pass();
	void createVertexInputLayout(const PipelineDesc* desc,
		std::vector<VkVertexInputBindingDescription>& binding_descriptions,
		std::vector<VkVertexInputAttributeDescription>& attribute_descriptions);
};

}

#endif