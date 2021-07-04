#ifndef SPARKPLUG_LOGICALDEVICE_H
#define SPARKPLUG_LOGICALDEVICE_H

#include "../Allocator/Allocator.h"
#include "../Instance/Instance.h"
#include "../PhysicalDevice/PhysicalDevice.h"

#include <vulkan/vulkan.h>


class LogicalDevice {

public:

    VkDevice vkDevice;

    //shadow of data in PhysicalDevice but as most object only require a logical device its better to have it here
    uint32_t graphicsFamilyIndex;
    uint32_t computeFamilyIndex;
    uint32_t transferFamilyIndex;

    std::vector<VkQueue> graphicsQueues;
    std::vector<VkQueue> computeQueues;
    std::vector<VkQueue> transferQueues;

    LogicalDevice();

    LogicalDevice* create(const Allocator &allocator, const Instance& instance, const PhysicalDevice& physicalDevice);

    LogicalDevice* destroy(const Allocator &allocator);
};


#endif //SPARKPLUG_LOGICALDEVICE_H
