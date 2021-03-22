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


struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	int graphicsFamilyIndex = -1;
	int computeFamilyIndex = -1;
	int presentationFamilyIndex = -1;

	bool validate() {
		return (graphicsFamilyIndex != -1 && presentationFamilyIndex != -1 && computeFamilyIndex != -1);
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

	/// <summary>
	/// Returns a arbitrary score for the given device baised on device properties
	/// </summary>
	/// <param name="vkPhysicalDevice">The physical device to be scored</param>
	/// <returns> Int: -1 if device limits fail otherwise returns device score</returns>
	int getDeviceScore(VkPhysicalDevice vkPhysicalDevice);

	/// <summary>
	/// Returns if the spawnchain surface supports any present modes and formats
	/// </summary>
	/// <param name="vkPhysicalDevice">The physical device to check</param>
	/// <param name="surface">The swapchain surface to check</param>
	/// <returns> True if present modes and formats are found, false otherwise</returns>
	static bool checkDeviceSwapchain(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

	/// <summary>
	/// Check device entensions, queues and swapchain for compatibillity
	/// </summary>
	/// <param name="vkPhysicalDevice">The device to check</param>
	/// <param name="surface">The swapchain surface to check</param>
	/// <param name="enableValidationLayers">if Validations layers are enabled</param>
	/// <returns>True if device is compatible</returns>
	bool checkPhysicalDeviceQueues(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

	/// <summary>
	/// Checks device support for each extensions specified in physicalDeviceExtensions
	/// </summary>
	/// <param name="vkPhysicalDevice">The deivce to check</param>
	/// <param name="enableValidationLayers">Whether to display extension info on console</param>
	/// <returns>True if all extensions are supported</returns>
	bool checkPhysicalDeviceExtensions(VkPhysicalDevice vkPhysicalDevice, bool enableValidationLayers);

	bool checkPhysicalDeviceCompatible(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface, bool enableValidationLayers);

public:

	/// <summary>
	/// Returns a strcut of swapchain support details 
	/// </summary>
	/// <param name="vkPhysicalDevice">The device to check support</param>
	/// <param name="surface">The surface to check agenst</param>
	/// <returns>SwapchainSupportDetails struct</returns>
	static SwapchainSupportDetails getSurfaceSwapchainSupport(VkPhysicalDevice& vkPhysicalDevice, VkSurfaceKHR& surface);

	/// <summary>
	/// Gets available queue familes from given device
	/// </summary>
	/// <param name="vkPhysicalDevice">The device to get the queue famalies from</param>
	/// <param name="surface">The surface for the present queue</param>
	/// <returns>A QueueFamilyIndices struct containg the relivant queue family indices</returns>
	static QueueFamilyIndices getQueueFamiles(VkPhysicalDevice& vkPhysicalDevice, VkSurfaceKHR& surface);

	std::vector<const char*> getDeviceExtensions();

	QueueFamilyIndices getQueueFamilyIndices();

	VkPhysicalDevice& getPhysicalDevice();

	
	/// <summary>
	/// Select the best detected physical device 
	/// </summary>
	/// <param name="vkInstance"></param>
	/// <param name="surface"></param>
	/// <param name="enableValidationLayers"></param>
	/// <returns></returns>
	VkPhysicalDevice select(VkInstance& vkInstance, VkSurfaceKHR& surface, bool enableValidationLayers);

	

	VkDeviceSize getMinUBOAllocation();
};

#endif