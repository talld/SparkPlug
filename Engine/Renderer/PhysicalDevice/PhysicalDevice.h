#ifndef PhysicalDevice_H
#define PhysicalDevice_H

#include "../Allocator/Allocator.h"
#include "../Instance/Instance.h"
#include "../Surface/Surface.h"

#include <vulkan/vulkan.h>
#include <vector>

#include <cstring>

class Surface;

class PhysicalDevice {

public:
    VkPhysicalDevice vkPhysicalDevice;
    VkPhysicalDeviceFeatures deviceFeatures;
    std::set<const char*> requiredExtensions;

    //queue family indices
    int graphicsFamilyIndex;
    int computeFamilyIndex;
    int transferFamilyIndex;


    PhysicalDevice();

    PhysicalDevice* create(const Allocator &allocator, const Instance& instance, const Surface& surface);

    PhysicalDevice* destroy();

    bool checkSurfaceSupported(const Surface& surface) const;

    /***
     *
     * @param extensionName
     * @return
     */
    PhysicalDevice* requireExtension(const char* extensionName);


private:


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

    /***
     * Returns a vector of the original devices with any device lacking required features removed
     * @param devices Devices to filer
     * @return Filtered vector
     */
    std::vector<VkPhysicalDevice> filterDevices(const std::vector<VkPhysicalDevice>& devices, const Surface& surface);

    /***
     * Returns true if device has graphics compute and transfer queue families
     * @param device Device to check
     * @return True if device supports all families
     */
    bool hasRequiredQueueFamilies(VkPhysicalDevice device, Surface surface) const;

    /***
     * Returns a vector of the available extensions on a given device
     * @param device Device to check
     * @return Vector of extension properties of all the support extensions
     */
    std::vector<VkExtensionProperties> getAvailableExtensions(const VkPhysicalDevice& device) const;

    /***
     * Returns if a device supports all the extensions provided
     * @param required Set of c strings extension names to check
     * @param device Device to check
     * @return True if all extensions are supported
     */
    bool supportsExtensions(VkPhysicalDevice device, const std::set<const char*>& required) const;

    bool supportsSurface(const VkPhysicalDevice& device, const Surface& surface) const;
};

#endif //PhysicalDevice_H
