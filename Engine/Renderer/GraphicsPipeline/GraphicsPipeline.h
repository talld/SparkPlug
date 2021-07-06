#ifndef SPARKPLUG_GRAPHICSPIPELINE_H
#define SPARKPLUG_GRAPHICSPIPELINE_H

#include "../Allocator/Allocator.h"
#include "../LogicalDevice/LogicalDevice.h"
#include "../Swapchain/Swapchain.h"
#include "../Shader/Shader.h"

#include <vulkan/vulkan.h>

class GraphicsPipeline {

public:
    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;

    std::vector<VkPipelineShaderStageCreateInfo> shaders;

    GraphicsPipeline();

    GraphicsPipeline *create(const Allocator &allocator, const LogicalDevice &device, const Swapchain &swapchain);

    GraphicsPipeline *destroy(const Allocator &allocator, const LogicalDevice &device);

    GraphicsPipeline *createLayout(const Allocator &allocator, const LogicalDevice &device);

    GraphicsPipeline *addShaderStage(const Shader &shader, VkShaderStageFlagBits shaderStage);
};


#endif //SPARKPLUG_GRAPHICSPIPELINE_H