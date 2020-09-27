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
#include <Renderer/LogicalDevice/LogicalDevice.h>
#include <Renderer/Window/Window.h>
#include <Renderer/RenderPass/RenderPass.h>

class Instnace;

class Window;

class PhysicalDevice;

class LogicalDeivce;

class RenderPass;

struct SwapchainImage {
	VkImage image;
	VkImageView imageView;
};

struct SwapchainDetails {
	VkExtent2D extent2D;
	VkSurfaceFormatKHR format;
	VkPresentModeKHR presentMode;
	std::vector<SwapchainImage> swapchainImages;
	std::vector<VkFramebuffer> frameBuffers;
};

class Swapchain {

private:

	SwapchainDetails swapchainDetails;

	VkSwapchainKHR vkSwapchain;

	VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, Window window);

	VkPresentModeKHR selectSwapchainPresentMode(std::vector<VkPresentModeKHR> presentModes);

	VkFormat selectSwapchainFormat(VkSurfaceFormatKHR format, std::vector<VkSurfaceFormatKHR> availableFormats);

	VkColorSpaceKHR selectSwapchainColorSpace(VkSurfaceFormatKHR format, std::vector<VkSurfaceFormatKHR> availableFormats);

	VkSurfaceFormatKHR selectSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);

	VkImageView createImageView(LogicalDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

public:


	void createFrameBuffers(VkDevice vkLogicalDevice, RenderPass renderPass);


	void create(PhysicalDevice physicalDevice, LogicalDevice logicalDevice, VkSurfaceKHR surface, Window window);

	VkSwapchainKHR getSwapchain();

	SwapchainDetails getSwapchainDetails();

	void destroy(VkDevice logicalDevice);
};