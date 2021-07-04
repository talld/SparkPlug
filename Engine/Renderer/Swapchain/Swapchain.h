//
// Created by talldie on 30/06/2021.
//

#ifndef SPARKPLUG_SWAPCHAIN_H
#define SPARKPLUG_SWAPCHAIN_H

#include "../Allocator/Allocator.h"
#include "../LogicalDevice/LogicalDevice.h"
#include "../Surface/Surface.h"

#include <vulkan/vulkan.h>
#include <algorithm>

class Swapchain {

public:
    VkSwapchainKHR vkSwapchain;

    VkSurfaceFormatKHR format;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;

    Swapchain();

    Swapchain *create(const Allocator &allocator, const Window &window, const Surface &surface,
                      const LogicalDevice &device);

    Swapchain *destroy(const Allocator &allocator, const LogicalDevice &device);

private:

    static VkSurfaceFormatKHR selectFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    static VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    static VkExtent2D selectExtent(VkSurfaceCapabilitiesKHR capabilities, const Window &window);

};


#endif //SPARKPLUG_SWAPCHAIN_H
