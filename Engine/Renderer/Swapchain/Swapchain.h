//
// Created by talldie on 30/06/2021.
//

#ifndef SPARKPLUG_SWAPCHAIN_H
#define SPARKPLUG_SWAPCHAIN_H

#include <vulkan/vulkan.h>

class Swapchain {

    VkSwapchainKHR vkSwapchain;

    Swapchain();

    Swapchain* create();

    Swapchain* destroy();

};


#endif //SPARKPLUG_SWAPCHAIN_H
