#ifndef SPARKPLUG_RENDERPASS_H
#define SPARKPLUG_RENDERPASS_H

#include "../Allocator/Allocator.h"
#include "../LogicalDevice/LogicalDevice.h"
#include "../Swapchain/Swapchain.h"

#include <vulkan/vulkan.h>
#include <array>

class Renderpass {

public:
    VkRenderPass vkRenderPass;

    Renderpass *create(const Allocator &allocator, const LogicalDevice &device, const Swapchain &swapchain);

    Renderpass *destroy(const Allocator &allocator, const LogicalDevice &device);

};


#endif //SPARKPLUG_RENDERPASS_H
