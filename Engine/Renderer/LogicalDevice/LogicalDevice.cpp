#include "LogicalDevice.h"

LogicalDevice::LogicalDevice() {
    vkDevice = nullptr;
    graphicsQueues = std::vector<VkQueue>();
    computeQueues = std::vector<VkQueue>();
    transferQueues = std::vector<VkQueue>();
}

LogicalDevice *LogicalDevice::create(const Allocator &allocator, const Instance &instance,
                                     const PhysicalDevice &physicalDevice,
                                     const Surface &surface) {

    uint32_t queueCount = surface.imageCount;

    graphicsQueues.resize( queueCount);
    computeQueues.resize(queueCount);
    transferQueues.resize(queueCount);

    graphicsFamilyIndex = physicalDevice.graphicsFamilyIndex;
    computeFamilyIndex = physicalDevice.computeFamilyIndex;
    transferFamilyIndex = physicalDevice.transferFamilyIndex;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::vector<float> priorities = std::vector<float>(queueCount,1.0f);

    for(const auto uniqueQueueIndex : std::set<uint32_t>{graphicsFamilyIndex,
                                                         computeFamilyIndex,
                                                         transferFamilyIndex})
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo;
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = uniqueQueueIndex;
        graphicsQueueCreateInfo.pQueuePriorities = priorities.data();
        graphicsQueueCreateInfo.queueCount = queueCount;
        graphicsQueueCreateInfo.flags = 0;
        graphicsQueueCreateInfo.pNext = nullptr;

        queueCreateInfos.push_back(graphicsQueueCreateInfo);
    }

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    // deprecated
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledLayerCount = 0;

    auto extens = std::vector<const char*>(physicalDevice.requiredExtensions.begin(), physicalDevice.requiredExtensions.end());
    createInfo.ppEnabledExtensionNames = extens.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extens.size());

    createInfo.pEnabledFeatures = &physicalDevice.deviceFeatures;


    createInfo.flags = 0;
    createInfo.pNext = nullptr;

    VkResult res = vkCreateDevice(physicalDevice.vkPhysicalDevice, &createInfo, allocator.allocationCallbacksPtr, &vkDevice);
    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create logicalDevice");
    }

    for(int i = 0; i < graphicsQueues.size(); i++)
        vkGetDeviceQueue(vkDevice, graphicsFamilyIndex, i, &graphicsQueues[i]);

    for(int i = 0; i < computeQueues.size(); i++)
        vkGetDeviceQueue(vkDevice, computeFamilyIndex, i, &graphicsQueues[i]);

    for(int i = 0; i < transferQueues.size(); i++)
        vkGetDeviceQueue(vkDevice, transferFamilyIndex, i, &graphicsQueues[i]);

    return this;
}

LogicalDevice *LogicalDevice::destroy(const Allocator &allocator) {

    vkDestroyDevice(vkDevice, allocator.allocationCallbacksPtr);
    return this;
}