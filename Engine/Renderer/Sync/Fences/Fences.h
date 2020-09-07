#pragma once
#define GLFW_INCLUDE_VULKAN
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





struct SwapchainFences {
	VkFence imagesInFlight;
};

struct GraphicFences {
	VkFence renderFence;
};

class Fences
{
private:

	std::vector<SwapchainFences> swapchainFences;
	std::vector<GraphicFences> graphicFences;
public:

	void create(VkDevice vkLogicalDevice, int maxBufferedImages);

	std::vector<SwapchainFences> getSwapchainFences();

	std::vector<GraphicFences> getGraphicsFences();
	
	VkFence getGraphicsFence(int index);

	VkFence* getGraphicsFenceP(int index);

	void destroy(VkDevice vkLogicalDevice, int maxBufferedImages);

};

