#ifndef SPARKPLUG_COMPUTEPIPELINE_H
#define SPARKPLUG_COMPUTEPIPELINE_H

#include "../Allocator/Allocator.h"
#include "../LogicalDevice/LogicalDevice.h"
#include "../Swapchain/Swapchain.h"

#include <vulkan/vulkan.h>

class ComputePipeline {

    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;

    ComputePipeline* create(const Allocator& allocator, const LogicalDevice& device, const Swapchain& swapchain);

    ComputePipeline* destroy(const Allocator& allocator, const LogicalDevice& device);


   VkPipelineLayout createLayout(const Allocator &allocator, const LogicalDevice &device);
};


#endif //SPARKPLUG_COMPUTEPIPELINE_H
