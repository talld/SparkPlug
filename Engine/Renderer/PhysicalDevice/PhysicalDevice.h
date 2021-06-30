#ifndef PhysicalDevice_H
#define PhysicalDevice_H

#include "../Allocator/Allocator.h"
#include "../Instance/Instance.h"

#include <vulkan/vulkan.h>
#include <vector>

class PhysicalDevice {


public:
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceLimits deviceLimits;

    PhysicalDevice* create(Allocator allocator, Instance instance);

private:

    /***
     * Returns a vector of the original devices with any device lacking required features removed
     * @param devices Devices to filer
     * @return Filtered vector
     */
    std::vector<VkPhysicalDevice> filterDevices(std::vector<VkPhysicalDevice> devices);

    /***
     * Returns true if device has graphics compute and transfer queue families
     * @param device Device to check
     * @return True if device supports all families
     */
    bool hasRequiredQueueFamilies(VkPhysicalDevice device);

    /***
     * Selects a device from the list according to some arbitrary checks
     * @param devices Devices to select from
     * @return selected Device
     */
    VkPhysicalDevice select(std::vector<VkPhysicalDevice> devices);
};

#endif //PhysicalDevice_H
