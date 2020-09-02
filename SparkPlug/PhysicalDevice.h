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

class LogicalDeivce;

class Swapchain;

class RenderPass;

class GraphicsPipeline;

class PhysicalDevice {

private:

	const std::vector<const char*> physicalDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

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

	QueueFamilyIndices indices;

	VkPhysicalDevice device;

	int getDeviceScore(VkPhysicalDevice vkPhysicalDevice) {

		int score = -1;

		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

		//score device
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += (deviceProperties.limits.maxImageDimension2D + deviceProperties.limits.maxFragmentInputComponents + deviceProperties.limits.maxGeometryOutputVertices) * 2;
		}
		else {
			score += (deviceProperties.limits.maxImageDimension2D + deviceProperties.limits.maxFragmentInputComponents + deviceProperties.limits.maxGeometryOutputVertices);
		}

		return score;
	}


	bool checkDeviceSwapChain(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
		SwapChainSupportDetails  swapChainSupport = getSurfaceSwapchainSupport(vkPhysicalDevice, surface);
		return !swapChainSupport.presentModes.empty() && !swapChainSupport.formats.empty();
	}

	bool checkPhysicalDeviceQueues(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {

		QueueFamilyIndices testedIndices = getQueueFamiles(vkPhysicalDevice, surface);
		if (testedIndices.validate()) {
			return true;
		}
		return false;
	}

	bool checkPhysicalDeviceExtemsions(VkPhysicalDevice vkPhysicalDevice, bool enableValidationLayers) {
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);

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

	bool checkPhysicalDeviceCompatible(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface, bool enableValidationLayers) {
		return (checkPhysicalDeviceExtemsions(vkPhysicalDevice, enableValidationLayers) && checkPhysicalDeviceQueues(vkPhysicalDevice, surface) && checkDeviceSwapChain(vkPhysicalDevice,surface));
	}

public:


		std::vector<const char*> getDeviceExtensions() {
			return physicalDeviceExtensions;
		}

		QueueFamilyIndices getQueueFamilyIndices() {
			return indices;
		}

		VkPhysicalDevice getPhysicalDevice() {
			return device;
		}

		SwapChainSupportDetails getSurfaceSwapchainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
			SwapChainSupportDetails supportDetails;

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

		void selectPhysicalDevice(VkInstance vkInstance, VkSurfaceKHR surface, bool enableValidationLayers) {

			uint32_t physDeviceCount = 0; //physical device count
			vkEnumeratePhysicalDevices(vkInstance, &physDeviceCount, nullptr);

			if (physDeviceCount == 0) {
				throw std::runtime_error("failed to locate a physical device");
			}

			std::vector<VkPhysicalDevice> physicalDevices(physDeviceCount);
			vkEnumeratePhysicalDevices(vkInstance, &physDeviceCount, physicalDevices.data());

			std::vector<VkPhysicalDevice> compatibleDevices(physDeviceCount);

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

			if (highestDevice == nullptr || highest == -1) {
				throw std::runtime_error("Failed to evaluate devices");
			}

			device = highestDevice;
			indices = getQueueFamiles(device, surface);
		}

		QueueFamilyIndices getQueueFamiles(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
			QueueFamilyIndices deviceQueueFamilyIndices;
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies) {
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					deviceQueueFamilyIndices.graphicsFamilyIndex = i;
					break;
				}

				i++;
			}
			i = 0;
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
};