#ifndef Instance_H
#define Instance_H

#include "../Allocator/Allocator.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <set>


class Window;

class Instance {

public:

    VkInstance vkInstance;

    std::set<const char*> requiredExtensions;
    VkApplicationInfo applicationInfo;
    VkInstanceCreateInfo createInfo;

    Instance* create(Allocator allocator);

    Instance* destroy(Allocator allocator);

    Instance* requireExtension(const char* extensionName);

private:

    Instance* getRequiredExtensions();

    std::vector<VkExtensionProperties> getAvailableExtensions();

    bool checkRequiredExtensions(std::set<const char*> requiredExtensions);
};

#endif