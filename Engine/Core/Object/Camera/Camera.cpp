#include "Camera.h"

void Camera::create(Swapchain swapchain, DescriptorSet* descriptorSet) {
	this->descriptorSet = descriptorSet;
	this->extent = swapchain.getSwapchainDetails().extent2D;
	this->mvp.projection = glm::perspective(glm::radians(45.0f), (float)extent.width / (float)extent.height, 0.1f, 100.0f);
	this->mvp.view = glm::lookAt(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->mvp.model = glm::mat4(1.0f);

	this->mvp.projection[1][1] *= -1;
}

void Camera::update() {
	descriptorSet->write(mvp);
}

VkDescriptorSet* Camera::getDescriptorSet(int i) {
	return descriptorSet->getDescriptorSet(i);
}
