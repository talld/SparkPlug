#ifndef PhysicalDevice_H
#define PhysicalDevice_H

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


struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	int graphicsFamilyIndex = -1;
	int presentationFamilyIndex = -1;

	bool validate() {
		return (graphicsFamilyIndex != -1 && presentationFamilyIndex != -1);
	}
};


class PhysicalDevice {

private:

	const std::vector<const char*> physicalDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceSize minUBOAllocation;


	QueueFamilyIndices indices;

	VkPhysicalDevice device;

	int getDeviceScore(VkPhysicalDevice vkPhysicalDevice);


	bool checkDeviceSwapchain(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

	bool checkPhysicalDeviceQueues(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

	bool checkPhysicalDeviceExtensions(VkPhysicalDevice vkPhysicalDevice, bool enableValidationLayers);

	bool checkPhysicalDeviceCompatible(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface, bool enableValidationLayers);

public:


	std::vector<const char*> getDeviceExtensions();

	QueueFamilyIndices getQueueFamilyIndices();

	VkPhysicalDevice getPhysicalDevice();

	SwapChainSupportDetails getSurfaceSwapchainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

	VkPhysicalDevice select(VkInstance vkInstance, VkSurfaceKHR surface, bool enableValidationLayers);

	QueueFamilyIndices getQueueFamiles(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

	VkDeviceSize getMinUBOAllocation();
};

#endif