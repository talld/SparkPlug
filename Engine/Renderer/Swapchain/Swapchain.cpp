#include "Swapchain.h"


Swapchain::Swapchain(){

    vkSwapchain = nullptr;
    format = {};
    presentMode;
    extent = {};
}

Swapchain *Swapchain::create(const Allocator &allocator, const Window &window, const Surface &surface,
                             const LogicalDevice &device) {

    // sel surface data
    format = selectFormat(surface.availableFormats);

    presentMode = selectPresentMode(surface.availablePresentModes);

    extent = selectExtent(surface.capabilities, window);

    // sel image count
    uint32_t imageCount = surface.capabilities.minImageCount;

    if(imageCount == 0){  // if no defined min
        imageCount = 2;
    }

    imageCount = std::min(imageCount, surface.capabilities.maxImageCount);


    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    createInfo.surface = surface.vkSurface;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.presentMode = presentMode;
    createInfo.imageExtent = extent;

    createInfo.minImageCount = imageCount;
    createInfo.imageArrayLayers = 1;                                        // TODO stero 3d support;

    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = surface.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.queueFamilyIndexCount = 0;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult res = vkCreateSwapchainKHR(device.vkDevice, &createInfo, allocator.allocationCallbacksPtr,&vkSwapchain);

    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create Swapchain");
    }

    return this;
}

Swapchain *Swapchain::destroy(const Allocator &allocator, const LogicalDevice &device) {
    vkDestroySwapchainKHR(device.vkDevice, vkSwapchain, allocator.allocationCallbacksPtr);
    return this;
}

/////////////////////////
/// Private Functions ///
/////////////////////////

VkSurfaceFormatKHR Swapchain::selectFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {

    VkSurfaceFormatKHR chosenFormat{};

    if(availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED){  //all formats available
        chosenFormat = { VK_FORMAT_B8G8R8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR };

    }else {

        for (const auto &format : availableFormats) {

            if(format.format == VK_FORMAT_B8G8R8A8_SRGB || format.format == VK_FORMAT_R8G8B8A8_SRGB
               && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){

                chosenFormat = format;
                break;

            }else if(format.format == VK_FORMAT_B8G8R8A8_UNORM || format.format == VK_FORMAT_R8G8B8A8_UNORM
                     && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){

                chosenFormat = format;
                continue;               //keep going over the other formats to see if they're superior
            }

        }

        //if a format has still not been found
        if(chosenFormat.format == VK_FORMAT_UNDEFINED){
            //anything will do
           chosenFormat = *&availableFormats[0];        //clang was complaining that the local var may escape the function
        }

    }

    return chosenFormat;
}

VkPresentModeKHR Swapchain::selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

    //TODO: options for tearing modes (vsync off)

    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    for(const auto& mode : availablePresentModes){
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR){
            chosenPresentMode = mode;
            break;
        }
    }

    return chosenPresentMode;
}

VkExtent2D Swapchain::selectExtent(VkSurfaceCapabilitiesKHR capabilities, const Window &window) {

    VkExtent2D chosenExtent{};

    if(capabilities.minImageExtent.width != UINT32_MAX &&
    capabilities.minImageExtent.height != UINT32_MAX){

        chosenExtent = capabilities.currentExtent;

    }else{

        int width, height;
        glfwGetFramebufferSize(window.glfwWindow, &width, &height);
        chosenExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        //clamp the values

        //floor
        chosenExtent.width = std::min(capabilities.maxImageExtent.width, chosenExtent.width);
        chosenExtent.height = std::min(capabilities.maxImageExtent.height, chosenExtent.height);

        //ceil
        chosenExtent.width = std::max(capabilities.minImageExtent.width, chosenExtent.width);
        chosenExtent.height = std::max(capabilities.minImageExtent.height, chosenExtent.height);

    }

    return chosenExtent;
}
