#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <set>
#include <vector>
#include <array>
#include <optional>
#include <fstream>

class Instnace;

class Window;

class PhysicalDevice;

class Swapchain;

class RenderPass;

class GraphicsPipeline;


class LogicalDevice {
private:
	struct Queues {
		VkQueue graphicsQueue;
		VkQueue presentationQueue;
	};

	Queues queues;

	VkDevice vkLogicalDevice;

public:
	void createLogicalDevice(PhysicalDevice physicalDevice) {

		const std::vector<const char*> physicalDeviceExtensions = physicalDevice.getDeviceExtensions();

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		std::set<int> uniqueQueueFamilies = { physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex , physicalDevice.getQueueFamilyIndices().presentationFamilyIndex };

		float qPriority = 1.0f;
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

	VkDevice getLogicalDevice() {
		return vkLogicalDevice;
	}

	Queues getQueues() {
		return queues;
	}

	void destory() {
		vkDestroyDevice(vkLogicalDevice, nullptr);
	}

};