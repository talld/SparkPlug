#include "Fences.h"

inline void Fences::create(VkDevice vkLogicalDevice, int maxBufferedImages) {
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

inline std::vector<SwapchainFences> Fences::getSwapchainFences() {
	return swapchainFences;
}

inline std::vector<GraphicFences> Fences::getGraphicsFences() {
	return graphicFences;
}

inline VkFence Fences::getGraphicsFence(int index) {
	return graphicFences[index].renderFence;
}

inline VkFence* Fences::getGraphicsFenceP(int index) {
	return &graphicFences[index].renderFence;
}

inline void Fences::destroy(VkDevice vkLogicalDevice, int maxBufferedImages) {
	for (size_t i = 0; i < maxBufferedImages; i++) {
		vkDestroyFence(vkLogicalDevice, graphicFences[i].renderFence, nullptr);
	}
}
