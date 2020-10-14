#include "Fences.h"

void Fences::create(VkDevice vkLogicalDevice, int maxBufferedImages) {
	graphicFences.resize(maxBufferedImages);

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < maxBufferedImages; i++) {
		if (vkCreateFence(vkLogicalDevice, &fenceCreateInfo, nullptr, &graphicFences[i].renderFence) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphic fences");
		}
	}
}

std::vector<SwapchainFences> Fences::getSwapchainFences() {
	return swapchainFences;
}

std::vector<GraphicFences> Fences::getGraphicsFences() {
	return graphicFences;
}

VkFence Fences::getGraphicsFence(int index) {
	return graphicFences[index].renderFence;
}

VkFence* Fences::getGraphicsFenceP(int index) {
	return &graphicFences[index].renderFence;
}

void Fences::destroy(VkDevice vkLogicalDevice, int maxBufferedImages) {
	for (size_t i = 0; i < maxBufferedImages; i++) {
		vkDestroyFence(vkLogicalDevice, graphicFences[i].renderFence, nullptr);
	}
}
