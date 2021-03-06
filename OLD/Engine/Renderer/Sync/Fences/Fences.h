#ifndef Fences_H
#define Fences_H

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

struct TransferFences {
	VkFence transferFinished;
};

class Fences
{
private:

	std::vector<SwapchainFences> swapchainFences;
	std::vector<GraphicFences> graphicFences;
	TransferFences transferFences;


public:

	void create(VkDevice vkLogicalDevice, int maxBufferedImages);

	std::vector<SwapchainFences> getSwapchainFences();

	VkFence getSwapchainFence(int index);

	VkFence* getSwapchainFenceP(int index);

	std::vector<GraphicFences> getGraphicsFences();
	
	VkFence getGraphicsFence(int index);

	VkFence* getGraphicsFenceP(int index);

	VkFence getTransferFinished(int index);

	VkFence* getTransferFinishedP(int index);

	void destroy(VkDevice vkLogicalDevice, int maxBufferedImages);

};

#endif