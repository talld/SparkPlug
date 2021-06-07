#include "Renderer.h"

void Renderer::bootRender() {
	initSettings();
	window.create();
	glfwWindow = window.getWindow();
	initVulkan();
}

void Renderer::enterMainLoop() {
	mainLoop();
}

void Renderer::terminate() {
	cleanUp();
}

static int meshCount = 0;

void Renderer::createMesh(Mesh* mesh, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices){
	mesh->create(physicalDevice, vkLogicalDevice, logicalDevice.getQueues().graphicsQueue, graphicsCommandPool, vertices, indices, allocator);
	descriptorSet.emplaceMesh(mesh->getUBO(), meshCount++);
}

void Renderer::bindCamera(Camera* camera){
	camera->create(swapchain, &descriptorSet);
	currentCamera = camera;
}

void Renderer::record(std::vector<Mesh>* meshes){
	commandBuffer.record(swapchain, renderPass, graphicsPipeline, currentCamera,meshes);
}

void Renderer::initVulkan() {
	instance.createInstance(enableValidationLayers);
	vkInstance = instance.getInstance();



	window.createSurface(vkInstance);
	surface = window.getSurface();

	vkPhysicalDevice = physicalDevice.select(vkInstance, surface, enableValidationLayers);



	logicalDevice.create(physicalDevice);
	vkLogicalDevice = logicalDevice.getLogicalDevice();

	allocator = createAllocator(vkPhysicalDevice, vkLogicalDevice, vkInstance);
	memUtilitesSettings.minUBOAllocation = physicalDevice.minUBOAllocation;

	swapchain.create(physicalDevice, logicalDevice, surface, window);
	vkSwapchain = swapchain.getSwapchain();
	renderPass.create(vkLogicalDevice, swapchain);

	vkDescriptorSetLayout = descriptorSet.createLayout(vkLogicalDevice);
	descriptorSet.createUniformBuffers(allocator, swapchain);
	vkDescriptorPool = descriptorPool.create(vkLogicalDevice, descriptorSet);
	descriptorSet.create(vkDescriptorPool);

	graphicsPipeline.create(vkLogicalDevice, swapchain, renderPass,vkDescriptorSetLayout);

	swapchain.createFrameBuffers(vkLogicalDevice, renderPass);

	commandPool.create(physicalDevice, vkLogicalDevice);
	graphicsCommandPool = commandPool.getCommandPools().graphicsCommandPool;

	commandBuffer.create(vkLogicalDevice, swapchain, graphicsCommandPool);
	semaphores.create(vkLogicalDevice, settings.maxBufferedImages);
	fences.create(vkLogicalDevice, settings.maxBufferedImages);
	setTransferSync(&fences, &currentFrame);

}

void Renderer::initSettings() {
	settings.maxBufferedImages = 3;

}

void Renderer::update() {
	glfwPollEvents();


}

void Renderer::mainLoop() {
	while (!glfwWindowShouldClose(glfwWindow)) {
		update();
		render();
	}
	vkDeviceWaitIdle(vkLogicalDevice);
}

void Renderer::render() {
	
	vkWaitForFences(vkLogicalDevice, 1, fences.getGraphicsFenceP(currentFrame), VK_TRUE, UINT64_MAX);
	vkResetFences(vkLogicalDevice, 1, fences.getGraphicsFenceP(currentFrame));
	uint32_t imageIndex;

	//next swapchain image index to imageIndex
	vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, UINT64_MAX, semaphores.getImagesAvailable(currentFrame), VK_NULL_HANDLE, &imageIndex);

	VkSemaphore waitSemaphores[] = { semaphores.getImagesAvailable(currentFrame) };
	VkSemaphore signalSemaphores[] = { semaphores.getRendersFinished(currentFrame) };
	
	//wait on pipeline color attachment before submiting an image to the commandBuffer
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer.getCommandBufferP(imageIndex);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(logicalDevice.getQueues().graphicsQueue, 1, &submitInfo, fences.getGraphicsFence(currentFrame)) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkSwapchainKHR swapchains[] = { vkSwapchain };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; //TODO: present results checking


	if (vkQueuePresentKHR(logicalDevice.getQueues().presentationQueue, &presentInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to present");
	}
	currentFrame = (currentFrame + 1) % settings.maxBufferedImages;

	currentFrame = (currentFrame + 1) % settings.maxBufferedImages;

}

void Renderer::cleanUp() {

	fences.destroy(vkLogicalDevice, settings.maxBufferedImages);

	semaphores.destroy(vkLogicalDevice, settings.maxBufferedImages);

	commandPool.destroy(vkLogicalDevice);

	descriptorSet.destroy();

	descriptorPool.destroy();

	graphicsPipeline.destroy(vkLogicalDevice);

	renderPass.destroy(vkLogicalDevice);

	swapchain.destroy(logicalDevice.getLogicalDevice());

	destroyAllocator(allocator);

	logicalDevice.destory();

	window.destroy(vkInstance);

	instance.destroy(enableValidationLayers);

}
