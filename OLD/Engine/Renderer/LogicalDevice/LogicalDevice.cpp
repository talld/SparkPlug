#include "LogicalDevice.h"

void LogicalDevice::create(PhysicalDevice& physicalDevice) {

	const std::vector<const char*> physicalDeviceExtensions = physicalDevice.getDeviceExtensions();

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;


	std::set<int> uniqueQueueFamilies = { physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex , physicalDevice.getQueueFamilyIndices().presentationFamilyIndex };

	//all queues just given a defualt priority of 1
	float qPriority = 1.0f;
	
	//create each unique queue from their indices
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &qPriority;

		deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = physicalDeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(physicalDevice.getPhysicalDevice(), &deviceCreateInfo, nullptr, &vkLogicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device");
	}

	vkGetDeviceQueue(vkLogicalDevice, physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex, 0, &queues.graphicsQueue);
	vkGetDeviceQueue(vkLogicalDevice, physicalDevice.getQueueFamilyIndices().presentationFamilyIndex, 0, &queues.presentationQueue);
}

VkDevice LogicalDevice::getLogicalDevice() {
	return vkLogicalDevice;
}

Queues LogicalDevice::getQueues() {
	return queues;
}

void LogicalDevice::destory() {
	vkDestroyDevice(vkLogicalDevice, nullptr);
}
