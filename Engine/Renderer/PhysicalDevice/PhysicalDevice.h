#ifndef PhysicalDevice_H
#define PhysicalDevice_H

#include "../Allocator/Allocator.h"
#include "../Instance/Instance.h"

#include <vulkan/vulkan.h>
#include <vector>

class PhysicalDevice {


public:
    VkPhysicalDevice vkPhysicalDevice;
    VkPhysicalDeviceFeatures deviceFeatures{};

    //queue family indices
    int graphicsFamilyIndex{};
    int computeFamilyIndex{};
    int transferFamilyIndex{};

    PhysicalDevice();

    PhysicalDevice* create(Allocator allocator, const Instance& instance);

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
    bool hasRequiredQueueFamilies(VkPhysicalDevice device) const;

    /***
     * Selects a device from the list according to some arbitrary checks
     * @param devices Devices to selectDevice from
     * @return selected Device
     */
    VkPhysicalDevice selectDevice(const std::vector<VkPhysicalDevice>& devices) const;

    /***
     * Selects the queue families indices for the chosen device
     * @return self
     */
    PhysicalDevice* selectQueueFamilies();
};

#endif //PhysicalDevice_H
