#ifndef SPARKPLUG_LOGICALDEVICE_H
#define SPARKPLUG_LOGICALDEVICE_H

#include "../Allocator/Allocator.h"
#include "../Instance/Instance.h"
#include "../PhysicalDevice/PhysicalDevice.h"

#include <vulkan/vulkan.h>


class LogicalDevice {

public:

    VkDevice vkDevice;

    std::vector<VkQueue> graphicsQueues;
    std::vector<VkQueue> computeQueues;
    std::vector<VkQueue> transferQueues;

    LogicalDevice();

    LogicalDevice* create(Allocator allocator, const Instance& instance, const PhysicalDevice& physicalDevice);

    LogicalDevice* destroy(Allocator allocator);
};


#endif //SPARKPLUG_LOGICALDEVICE_H
