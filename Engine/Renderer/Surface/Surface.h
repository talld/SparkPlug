#ifndef SPARKPLUG_SURFACE_H
#define SPARKPLUG_SURFACE_H

#include "../Window/Window.h"
#include "../Instance/Instance.h"
#include "../PhysicalDevice/PhysicalDevice.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class PhysicalDevice;

class Surface {

public:

    VkSurfaceKHR vkSurface;
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> availableFormats;
    std::vector<VkPresentModeKHR> availablePresentModes;

    Surface();

    Surface* create(const Allocator &allocator, const Instance& instance, const Window& window);

    Surface* destroy(const Allocator &allocator, const Instance& instance);

    Surface* evaluateCapabilities(const PhysicalDevice& device);

};


#endif //SPARKPLUG_SURFACE_H