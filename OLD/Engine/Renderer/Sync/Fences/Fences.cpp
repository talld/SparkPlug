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

	VkResult result = vkCreateFence(vkLogicalDevice, &fenceCreateInfo, nullptr, &transferFences.transferFinished);

	if(result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create transfer fence");
	}

}

std::vector<SwapchainFences> Fences::getSwapchainFences() {
	return swapchainFences;
}

VkFence Fences::getSwapchainFence(int index) {
	return swapchainFences[index].imagesInFlight;
}

VkFence* Fences::getSwapchainFenceP(int index) {
	return &swapchainFences[index].imagesInFlight;
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

VkFence Fences::getTransferFinished(int index) {
	return transferFences.transferFinished;
}

VkFence* Fences::getTransferFinishedP(int index) {
	return &transferFences.transferFinished;
}

void Fences::destroy(VkDevice vkLogicalDevice, int maxBufferedImages) {
	for (size_t i = 0; i < maxBufferedImages; i++) {
		vkDestroyFence(vkLogicalDevice, graphicFences[i].renderFence, nullptr);
	}
	vkDestroyFence(vkLogicalDevice, transferFences.transferFinished, nullptr);
}
