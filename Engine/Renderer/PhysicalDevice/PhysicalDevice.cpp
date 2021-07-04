#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice() {
    vkPhysicalDevice = {};
    deviceFeatures = {};

    graphicsFamilyIndex = -1;
    computeFamilyIndex = -1;
    transferFamilyIndex = -1;

    requiredExtensions = std::set<const char*>();
}


PhysicalDevice *PhysicalDevice::create(const Allocator &allocator, const Instance& instance, const Surface& surface) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.vkInstance ,&deviceCount, nullptr);

    if(deviceCount == 0){
        allocator.throwError(VK_ERROR_DEVICE_LOST, "Failed to find graphics device");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(instance.vkInstance ,&deviceCount, devices.data());

    requireExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    devices = filterDevices(devices, surface);

    if(devices.empty()){
        allocator.throwError(VK_ERROR_DEVICE_LOST, "Failed to find compatible device");
    }

    vkPhysicalDevice = selectDevice(devices);

#ifdef DEBUG

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);

    printf("Device selected: %s\n", properties.deviceName);

#endif

    selectQueueFamilies();

    return this;
}

PhysicalDevice *PhysicalDevice::destroy() {
    return this;
}

bool PhysicalDevice::checkSurfaceSupported(const Surface &surface) const {

    VkBool32 graphicsQueuePresentationSupported = false;

    //TODO: select new graphics queue family in the event that the select family does not support presentation
    vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, graphicsFamilyIndex, surface.vkSurface, &graphicsQueuePresentationSupported);

    return graphicsQueuePresentationSupported;
}

PhysicalDevice *PhysicalDevice::requireExtension(const char *extensionName) {
    requiredExtensions.insert(extensionName);
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

std::vector<VkPhysicalDevice> PhysicalDevice::filterDevices(const std::vector<VkPhysicalDevice>& devices, const Surface& surface) {

    std::vector<VkPhysicalDevice> filteredDevice;

    for(auto & device : devices) {

        VkPhysicalDeviceFeatures physicalDeviceFeatures{};
        vkGetPhysicalDeviceFeatures(device, &physicalDeviceFeatures);

        if( physicalDeviceFeatures.geometryShader
           && physicalDeviceFeatures.tessellationShader
           && hasRequiredQueueFamilies(device, surface)
           && supportsExtensions(device, requiredExtensions)
           && supportsSurface(device, surface)
        ){
            filteredDevice.push_back(device);
        }
    }

    return filteredDevice;
}

bool PhysicalDevice::hasRequiredQueueFamilies(VkPhysicalDevice device, Surface surface) const{

    uint32_t qFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFams(qFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, queueFams.data());

    bool graphicsFound = false;
    bool computeFound  = false;
    bool transferFound = false;

    VkBool32 presentFound = false;

    bool allFound      = false;

    int i = 0;
    for(const auto fam : queueFams){

        if(fam.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphicsFound = true;

        if(fam.queueFlags & VK_QUEUE_COMPUTE_BIT)
            computeFound = true;

        if(fam.queueFlags & VK_QUEUE_TRANSFER_BIT)
            transferFound = true;

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.vkSurface, &presentFound);

        i++;

        if(computeFound && graphicsFound && transferFound && presentFound){
            allFound = true;
            break;
        }
    }

    return  allFound;
}

std::vector<VkExtensionProperties> PhysicalDevice::getAvailableExtensions(VkPhysicalDevice const &device) const {

    uint32_t extenCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extenCount, nullptr);
    std::vector<VkExtensionProperties> extens(extenCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extenCount, extens.data());

    return extens;
}

bool PhysicalDevice::supportsExtensions(VkPhysicalDevice device, const std::set<const char*>& required) const {

    bool allSupported = false;

    std::vector<VkExtensionProperties> availableExtens = getAvailableExtensions(device);
    std::vector<const char*> extens = std::vector<const char*>(required.begin(), required.end());

    //TODO: merge this with near identical instance function

    #ifdef DEBUG

        printf("Available Device Extensions:\n");
        for(const auto exten : availableExtens)
            printf("----%s\n", exten.extensionName);

        printf("Required Device Extensions:\n");
        for(const auto exten : requiredExtensions)
            printf("----%s\n", exten);

    #endif

    for (auto exten = extens.end()-1; exten != extens.begin()-1; exten--) {
        for (const auto availableExten : availableExtens) {
            if (strncmp(*exten, availableExten.extensionName, strlen(availableExten.extensionName)) == 0)
                extens.erase(exten);
        }
    }

    if(!extens.empty()){

        allSupported = false;
        for (const auto exten : extens) {
            printf("Could not find Device Extension: %s\n", exten);
        }

    } else
        allSupported = true;

ret:
    return allSupported;

}

bool PhysicalDevice::supportsSurface(const VkPhysicalDevice& device, const Surface& surface) const{

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.vkSurface, &formatCount, nullptr);

    uint32_t modeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.vkSurface, &modeCount, nullptr);

    return modeCount && formatCount;
}


