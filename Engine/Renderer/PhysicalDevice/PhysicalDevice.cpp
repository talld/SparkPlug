#include <cstdlib>
#include "PhysicalDevice.h"

PhysicalDevice *PhysicalDevice::create(Allocator allocator, Instance instance) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.vkInstance ,&deviceCount, nullptr);

    if(deviceCount == 0){
        allocator.throwError(VK_ERROR_DEVICE_LOST, "Failed to find graphics device");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(instance.vkInstance ,&deviceCount, devices.data());


    filterDevices(devices);
    select(devices);

    return this;
}

/////////////////////////
/// Private Functions ///
/////////////////////////

VkPhysicalDevice PhysicalDevice::select(std::vector<VkPhysicalDevice> devices) {

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

bool PhysicalDevice::hasRequiredQueueFamilies(VkPhysicalDevice device) {

    uint32_t qFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFams(qFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, queueFams.data());

    bool graphicsFound = false;
    bool computeFound  = false;
    bool transferFound = false;

    bool allFound      = false;

    for(const auto fam : queueFams){

        if((fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            graphicsFound = true;

        if((fam.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
            computeFound = true;

        if((fam.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
            transferFound = true;

        if(computeFound && graphicsFound && transferFound){
            allFound = true;
            break;
        }
    }

    return  allFound;
}


