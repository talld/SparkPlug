#include "Semaphores.h"

void Semaphores::create(VkDevice vkLogicalDevice, int maxBufferedImages) {

	imagesAvailable.resize(maxBufferedImages);
	rendersFinished.resize(maxBufferedImages);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (size_t i = 0; i < maxBufferedImages; i++) {
		if (vkCreateSemaphore(vkLogicalDevice, &semaphoreCreateInfo, nullptr, &imagesAvailable[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vkLogicalDevice, &semaphoreCreateInfo, nullptr, &rendersFinished[i]) != VK_SUCCESS) {

			throw std::runtime_error("Failed to create graphic semaphores");
		}
	}
}

VkSemaphore Semaphores::getImagesAvailable(int index) {
	return imagesAvailable[index];
}

VkSemaphore Semaphores::getRendersFinished(int index) {
	return rendersFinished[index];
}





void Semaphores::destroy(VkDevice vkLogicalDevice, int maxBufferedImages) {
	for (size_t i = 0; i < maxBufferedImages; i++) {
		vkDestroySemaphore(vkLogicalDevice, rendersFinished[i], nullptr);
		vkDestroySemaphore(vkLogicalDevice, imagesAvailable[i], nullptr);
	}
}
