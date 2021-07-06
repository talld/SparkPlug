#ifndef SPARKPLUG_SHADER_H
#define SPARKPLUG_SHADER_H

#include "../Allocator/Allocator.h"
#include "../LogicalDevice/LogicalDevice.h"

#include <vulkan/vulkan.h>

class Shader{

public:

    VkShaderModule vkShaderModule;

    Shader();

    Shader *create(const Allocator &allocator, const LogicalDevice &device, const std::string &shaderFileName);

    Shader* destroy(const Allocator &allocator, const LogicalDevice &device);

};

#endif