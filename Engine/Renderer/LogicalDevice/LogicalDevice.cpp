//
// Created by talldie on 30/06/2021.
//

#include "LogicalDevice.h"

LogicalDevice::LogicalDevice() {
    vkDevice = nullptr;
    graphicsQueues = std::vector<VkQueue>();
    computeQueues = std::vector<VkQueue>();
    transferQueues = std::vector<VkQueue>();
}

LogicalDevice *LogicalDevice::create(Allocator allocator, const Instance& instance, const PhysicalDevice& physicalDevice) {

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float priority = 1.0f;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo;
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = physicalDevice.graphicsFamilyIndex;
    graphicsQueueCreateInfo.pQueuePriorities = &priority;
    graphicsQueueCreateInfo.queueCount = 1;                                         //when pools start to be allocated they'll be one queue per pool
    graphicsQueueCreateInfo.flags = 0;
    graphicsQueueCreateInfo.pNext = nullptr;

    queueCreateInfos.push_back(graphicsQueueCreateInfo);

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    // deprecated
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledLayerCount = 0;

    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.enabledExtensionCount = 0;

    createInfo.pEnabledFeatures = &physicalDevice.deviceFeatures;


    createInfo.flags = 0;
    createInfo.pNext = nullptr;

    VkResult res = vkCreateDevice(physicalDevice.vkPhysicalDevice, &createInfo, allocator.allocationCallbacksPtr, &vkDevice);
    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create logicalDevice");
    }

    return this;
}

LogicalDevice *LogicalDevice::destroy(Allocator allocator) {
    vkDestroyDevice(vkDevice, allocator.allocationCallbacksPtr);
    return this;
}