#include "PhysicalDevice.h"


int PhysicalDevice::getDeviceScore(VkPhysicalDevice vkPhysicalDevice) {

	int score = -1;

	VkPhysicalDeviceProperties deviceProperties{};
	vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

	//score device
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += (deviceProperties.limits.maxImageDimension2D + deviceProperties.limits.maxFragmentInputComponents + deviceProperties.limits.maxGeometryOutputVertices) * 4;
	}
	else {
		score += (deviceProperties.limits.maxImageDimension2D + deviceProperties.limits.maxFragmentInputComponents + deviceProperties.limits.maxGeometryOutputVertices);
	}

	return score;
}


SwapchainSupportDetails PhysicalDevice::getSurfaceSwapchainSupport(VkPhysicalDevice& vkPhysicalDevice, VkSurfaceKHR& surface) {
	SwapchainSupportDetails supportDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, surface, &supportDetails.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, nullptr);

	if (formatCount > 0) {
		supportDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, supportDetails.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount > 0) {
		supportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount, supportDetails.presentModes.data());
	}

	return supportDetails;
}


bool PhysicalDevice::checkDeviceSwapchain(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
	SwapchainSupportDetails  swapChainSupport = getSurfaceSwapchainSupport(vkPhysicalDevice, surface);
	return !swapChainSupport.presentModes.empty() && !swapChainSupport.formats.empty();
}

bool PhysicalDevice::checkPhysicalDeviceQueues(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {

	QueueFamilyIndices testedIndices = getQueueFamiles(vkPhysicalDevice, surface);
	if (testedIndices.validate()) {
		return true;
	}
	return false;
}

bool PhysicalDevice::checkPhysicalDeviceExtensions(VkPhysicalDevice vkPhysicalDevice, bool enableValidationLayers) {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);

	//if device has extensions properties enumeration failed
	if (extensionCount == 0) {
		return false;
	}

	std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, deviceExtensions.data());

	for (const auto& requiredExtension : physicalDeviceExtensions) {
		bool hasExtension = false;
		for (const auto& deviceExtension : deviceExtensions) {
			if (strcmp(requiredExtension, deviceExtension.extensionName) == 0) {
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension) {
			if (enableValidationLayers) {
				VkPhysicalDeviceProperties deviceProperites{};
				vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperites);
				std::cout << '\n' << deviceProperites.deviceName << ' ' << deviceProperites.deviceID << '\t' << "failed extension check: " << requiredExtension << '\n';

			}
			return false;
		}
	}
	return true;
}

bool PhysicalDevice::checkPhysicalDeviceCompatible(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface, bool enableValidationLayers) {
	return (checkPhysicalDeviceExtensions(vkPhysicalDevice, enableValidationLayers) && checkPhysicalDeviceQueues(vkPhysicalDevice, surface) && checkDeviceSwapchain(vkPhysicalDevice, surface));
}

std::vector<const char*> PhysicalDevice::getDeviceExtensions() {
	return physicalDeviceExtensions;
}

QueueFamilyIndices PhysicalDevice::getQueueFamilyIndices() {
	return indices;
}

VkPhysicalDevice& PhysicalDevice::getPhysicalDevice() {
	return device;
}



VkPhysicalDevice PhysicalDevice::select(VkInstance& vkInstance, VkSurfaceKHR& surface, bool enableValidationLayers){

	uint32_t physDeviceCount = 0; //physical device count
	vkEnumeratePhysicalDevices(vkInstance, &physDeviceCount, nullptr);

	if (physDeviceCount == 0) {
		throw std::runtime_error("failed to locate a physical device");
	}

	std::vector<VkPhysicalDevice> physicalDevices(physDeviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &physDeviceCount, physicalDevices.data());

	std::vector<VkPhysicalDevice> compatibleDevices(physDeviceCount);
	
	//out of the enumerated devices are any compatible
	bool foundCompatible = false;
	for (const auto& physicalDevice : physicalDevices) {
		if (checkPhysicalDeviceCompatible(physicalDevice, surface, enableValidationLayers)) {
			compatibleDevices.push_back(physicalDevice);
			foundCompatible = true;
		}
	}

	if (foundCompatible == false) {
		throw std::runtime_error("Failed to find compatible device");
	}

	//find highest scoring device
	int highest = -1;
	VkPhysicalDevice highestDevice = nullptr;
	for (const auto& compatibleDevice : compatibleDevices) {
		if (compatibleDevice) {
			int score = getDeviceScore(compatibleDevice);
			if (highest < score) {
				highest = score;
				highestDevice = compatibleDevice;
			}
		}
	}
	VkPhysicalDeviceProperties deviceProperties{};
	vkGetPhysicalDeviceProperties(highestDevice, &deviceProperties);

	std::cout << (deviceProperties.deviceName);

	//if could not find device
	if (highestDevice == nullptr || highest == -1) {
		throw std::runtime_error("Failed to evaluate devices");
	}

	this->device = highestDevice;
	
	//get info from selected device
	this->indices = getQueueFamiles(device, surface);
	this->minUBOAllocation = deviceProperties.limits.minUniformBufferOffsetAlignment;
	return highestDevice;
}


QueueFamilyIndices PhysicalDevice::getQueueFamiles(VkPhysicalDevice& vkPhysicalDevice, VkSurfaceKHR& surface){
	
	QueueFamilyIndices deviceQueueFamilyIndices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;

	//find graphics queue
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			deviceQueueFamilyIndices.graphicsFamilyIndex = i;
			break;
		}
		i++;
	}
	i = 0;

	//find computer queue
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			deviceQueueFamilyIndices.computeFamilyIndex = i;
			break;
		}
		i++;
	}
	i = 0;

	//find present queue
	for (const auto& queueFamily : queueFamilies) {
		VkBool32 surfaceSupported;
		vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface, &surfaceSupported);
		if (surfaceSupported) {
			deviceQueueFamilyIndices.presentationFamilyIndex = i;
			break;
		}

		i++;
	}
	return deviceQueueFamilyIndices;
}

VkDeviceSize PhysicalDevice::getMinUBOAllocation() {
	return minUBOAllocation;
}
