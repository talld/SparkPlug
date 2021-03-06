#include "Surface.h"

Surface::Surface() {
    
    vkSurface = nullptr;
    capabilities = {};
    availableFormats = std::vector<VkSurfaceFormatKHR>();
}

Surface *Surface::create(const Allocator &allocator, const Instance& instance, const Window& window) {
    VkResult res = glfwCreateWindowSurface(instance.vkInstance, window.glfwWindow, allocator.allocationCallbacksPtr, &vkSurface);

    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create surface");
    }

    return this;
}

Surface *Surface::destroy(const Allocator &allocator, const Instance& instance) {

    vkDestroySurfaceKHR(instance.vkInstance, vkSurface, allocator.allocationCallbacksPtr);
    return this;
}

Surface *Surface::evaluateCapabilities(const PhysicalDevice &device) {

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.vkPhysicalDevice, vkSurface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.vkPhysicalDevice, vkSurface, &formatCount, nullptr);

    availableFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.vkPhysicalDevice, vkSurface, &formatCount, availableFormats.data());


    uint32_t modeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.vkPhysicalDevice, vkSurface, &modeCount, nullptr);

    availablePresentModes.resize(modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.vkPhysicalDevice, vkSurface, &modeCount, availablePresentModes.data());


    imageCount = capabilities.minImageCount;

    if(imageCount == 0){  // if no defined min
        imageCount = 2;
    }

    imageCount = std::min(imageCount+1, capabilities.maxImageCount);

    return this;
}