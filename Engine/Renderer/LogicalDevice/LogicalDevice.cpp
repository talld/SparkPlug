#include "LogicalDevice.h"

LogicalDevice::LogicalDevice() {
    vkDevice = nullptr;
    graphicsQueues = std::vector<VkQueue>();
    computeQueues = std::vector<VkQueue>();
    transferQueues = std::vector<VkQueue>();
}

LogicalDevice *LogicalDevice::create(Allocator allocator, const Instance& instance, const PhysicalDevice& physicalDevice) {

    //TODO: allocate queues in accordance to pool size / swap-chain Images
    int queueCount = 1;

    graphicsQueues.resize( queueCount);
    computeQueues.resize(queueCount);
    transferQueues.resize(queueCount);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float priority = 1.0f;

    for(const auto uniqueQueueIndex : std::set<int>{physicalDevice.graphicsFamilyIndex,
                                     physicalDevice.computeFamilyIndex,
                                     physicalDevice.transferFamilyIndex})
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo;
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = uniqueQueueIndex;
        graphicsQueueCreateInfo.pQueuePriorities = &priority;
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
        vkGetDeviceQueue(vkDevice, physicalDevice.graphicsFamilyIndex, i, &graphicsQueues[i]);

    for(int i = 0; i < computeQueues.size(); i++)
        vkGetDeviceQueue(vkDevice, physicalDevice.computeFamilyIndex, i, &graphicsQueues[i]);

    for(int i = 0; i < transferQueues.size(); i++)
        vkGetDeviceQueue(vkDevice, physicalDevice.transferFamilyIndex, i, &graphicsQueues[i]);

    return this;
}

LogicalDevice *LogicalDevice::destroy(Allocator allocator) {

    vkDestroyDevice(vkDevice, allocator.allocationCallbacksPtr);
    return this;
}