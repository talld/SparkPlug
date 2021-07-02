#ifndef Instance_H
#define Instance_H

#include "../Allocator/Allocator.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <cstring>
#include <vector>
#include <set>


class Window;

class Instance {

public:

    VkInstance vkInstance;

    std::set<const char*> requiredExtensions;
    VkApplicationInfo applicationInfo;
    VkInstanceCreateInfo createInfo;

    Instance();

    Instance* create(Allocator allocator);

    Instance* destroy(Allocator allocator);

    /***
     * Add an extension to the required extension list
     * @param extensionName name of extension (char[256])
     * @return self
     */
    Instance* requireExtension(const char* extensionName);

private:

    Instance* getRequiredExtensions();

    std::vector<VkExtensionProperties> getAvailableExtensions();

    bool checkRequiredExtensions(const std::set<const char*>& requiredExtensions);
};

#endif