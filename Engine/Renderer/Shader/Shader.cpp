#include "Shader.h"
#include "../../Utilities/FileUtils.h"

Shader::Shader() {
    vkShaderModule = {};
}

Shader *Shader::create(const Allocator &allocator, const LogicalDevice &device, const std::string &shaderFileName) {

    auto shaderCode =  readFile(shaderFileName);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    createInfo.codeSize = shaderCode.size();

    VkResult res = vkCreateShaderModule(device.vkDevice, &createInfo, allocator.allocationCallbacksPtr, &vkShaderModule);

    if(res != VK_SUCCESS){
        allocator.throwError(res, strcat("Failed to create Shader module: ", shaderFileName.c_str()));
    }

    return this;
}

Shader *Shader::destroy(const Allocator &allocator, const LogicalDevice &device) {
    vkDestroyShaderModule(device.vkDevice, vkShaderModule, allocator.allocationCallbacksPtr);
    return this;
}


