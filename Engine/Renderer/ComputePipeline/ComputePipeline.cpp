#include "ComputePipeline.h"

ComputePipeline *
ComputePipeline::create(const Allocator &allocator, const LogicalDevice &device, const Swapchain &swapchain) {

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;



    VkComputePipelineCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

    createInfo.layout = vkPipelineLayout;

    createInfo.stage = shaderStageCreateInfo;

    createInfo.basePipelineIndex = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;

    vkCreateComputePipelines(device.vkDevice, nullptr, 1, &createInfo, allocator.allocationCallbacksPtr, &vkPipeline);

    return this;
}

ComputePipeline *ComputePipeline::destroy(const Allocator &allocator, const LogicalDevice &device) {
    return this;
}


VkPipelineLayout ComputePipeline::createLayout(const Allocator &allocator, const LogicalDevice &device) {

    VkPipelineLayout pipelineLayout;

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    vkCreatePipelineLayout(device.vkDevice, &createInfo, allocator.allocationCallbacksPtr, &pipelineLayout);

    return pipelineLayout;
}
