#include <cstdlib>
#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice() {
    vkPhysicalDevice = {};
    deviceFeatures = {};

    graphicsFamilyIndex = -1;
    computeFamilyIndex = -1;
    transferFamilyIndex = -1;
}


PhysicalDevice *PhysicalDevice::create(Allocator allocator, const Instance& instance) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.vkInstance ,&deviceCount, nullptr);

    if(deviceCount == 0){
        allocator.throwError(VK_ERROR_DEVICE_LOST, "Failed to find graphics device");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(instance.vkInstance ,&deviceCount, devices.data());

    filterDevices(devices);

    vkPhysicalDevice = selectDevice(devices);

    selectQueueFamilies();

    return this;
}

/////////////////////////
/// Private Functions ///
/////////////////////////

VkPhysicalDevice PhysicalDevice::selectDevice(const std::vector<VkPhysicalDevice>& devices) const{

    int64_t highestScore = INT64_MIN;
    VkPhysicalDevice chosenDevice;

    for(const auto device : devices){


        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        int64_t deviceScore =   deviceProperties.limits.maxDescriptorSetUniformBuffers                   // might give an indication on the VRAM
                        + (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU * 2000)  // bonus if it's it own vram
                        + deviceProperties.limits.maxComputeWorkGroupInvocations;                       // might give an indication on the shader core count

        if(deviceScore > highestScore) {
            highestScore = deviceScore;
            chosenDevice = device;
        }
    }

    return chosenDevice;
}

std::vector<VkPhysicalDevice> PhysicalDevice::filterDevices(std::vector<VkPhysicalDevice> devices) {

    for(auto device = devices.begin(); device != devices.end(); ++device) {

        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(*device, &physicalDeviceFeatures);

        if( !physicalDeviceFeatures.geometryShader
           || !physicalDeviceFeatures.tessellationShader
           || !hasRequiredQueueFamilies(*device)
        ){
            devices.erase(device);
        }
    }

    return devices;
}

bool PhysicalDevice::hasRequiredQueueFamilies(VkPhysicalDevice device) const{

    uint32_t qFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFams(qFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, queueFams.data());

    bool graphicsFound = false;
    bool computeFound  = false;
    bool transferFound = false;

    bool allFound      = false;

    for(const auto fam : queueFams){

        if(fam.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphicsFound = true;

        if(fam.queueFlags & VK_QUEUE_COMPUTE_BIT)
            computeFound = true;

        if(fam.queueFlags & VK_QUEUE_TRANSFER_BIT)
            transferFound = true;

        if(computeFound && graphicsFound && transferFound){
            allFound = true;
            break;
        }
    }

    return  allFound;
}

PhysicalDevice *PhysicalDevice::selectQueueFamilies() {

    uint32_t qFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &qFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFams(qFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &qFamilyCount, queueFams.data());

    bool graphicsFound = false;
    bool computeFound  = false;
    bool transferFound = false;

    bool allFound      = false;

    int index = 0;

    //TODO: Optimize family selection
    for(const auto fam : queueFams) {

        if (fam.queueCount > 0 && fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFound = true;
            graphicsFamilyIndex = index;
        }

        if (fam.queueCount > 0 && fam.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            computeFound = true;
            computeFamilyIndex = index;
        }

        if (fam.queueCount > 0 && fam.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            transferFound = true;
            transferFamilyIndex = index;
        }

        index++;

        if (computeFound && graphicsFound && transferFound) {
            allFound = true;
            break;
        }
    }

    return this;
}



