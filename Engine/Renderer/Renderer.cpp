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

 void Renderer::initVulkan() {
	instance.createInstance(enableValidationLayers);
	vkInstance = instance.getInstance();

	window.createSurface(vkInstance);
	surface = window.getSurface();

	physicalDevice.select(vkInstance, surface, enableValidationLayers);
	vkPhysicalDevice = physicalDevice.getPhysicalDevice();

	logicalDevice.create(physicalDevice);
	vkLogicalDevice = logicalDevice.getLogicalDevice();

	swapchain.create(physicalDevice, logicalDevice, surface, window);
	vkSwapchain = swapchain.getSwapchain();
	renderPass.create(vkLogicalDevice, swapchain);


	graphicsPipeline.create(vkLogicalDevice, swapchain, renderPass);

	swapchain.createFrameBuffers(vkLogicalDevice, renderPass);

	commandPool.create(physicalDevice, vkLogicalDevice);
	graphicsCommandPool = commandPool.getCommandPools().graphicsCommandPool;

	std::vector<Vertex> meshVertices1 = {
		{ { -0.1, -0.4, 0.0 },{ 1.0f,0.0f,0.0f } },
	{ { -0.1, 0.4, 0.0 },{ 0.0f, 1.0f, 0.0f } },
	{ { -0.9, 0.4, 0.0 },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.9, -0.4, 0.0 },{ 0.0f, 0.0f, 1.0f } },
	};

	std::vector<Vertex> meshVertices2 = {
		{ { 0.9, -0.3, 0.0 },{ 1.0f,0.0f,0.0f } },
	{ { 0.9, 0.1, 0.0 },{ 0.0f, 1.0f, 0.0f } },
	{ { 0.1, 0.3, 0.0 },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.1, -0.3, 0.0 },{ 0.0f, 0.0f, 1.0f } },
	};

	std::vector<uint32_t> meshIndices = {
		0,1,2,
		2,3,0
	};

	Mesh mesh1;
	mesh1.create(vkPhysicalDevice, vkLogicalDevice, logicalDevice.getQueues().graphicsQueue, graphicsCommandPool, &meshVertices1, &meshIndices);
	meshes.push_back(mesh1);
	Mesh mesh2;
	mesh2.create(vkPhysicalDevice, vkLogicalDevice, logicalDevice.getQueues().graphicsQueue, graphicsCommandPool, &meshVertices2, &meshIndices);
	meshes.push_back(mesh2);

	commandBuffer.create(vkLogicalDevice, swapchain, graphicsCommandPool);
	commandBuffer.record(swapchain, renderPass, graphicsPipeline, meshes);
	semaphores.create(vkLogicalDevice, settings.maxBufferedImages);
	fences.create(vkLogicalDevice, settings.maxBufferedImages);


	initCleanUp();
}

 void Renderer::initSettings() {
	settings.maxBufferedImages = 3;

}

void Renderer::update() {
	glfwPollEvents();


}

 void Renderer::render() {

	vkWaitForFences(vkLogicalDevice, 1, fences.getGraphicsFenceP(currentFrame), VK_TRUE, UINT64_MAX);
	vkResetFences(vkLogicalDevice, 1, fences.getGraphicsFenceP(currentFrame));
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, UINT64_MAX, semaphores.getImagesAvailable(currentFrame), VK_NULL_HANDLE, &imageIndex);

	VkSemaphore waitSemaphores[] = { semaphores.getImagesAvailable(currentFrame) };
	VkSemaphore signalSemaphores[] = { semaphores.getRendersFinished(currentFrame) };
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

	presentInfo.pResults = nullptr; // Optional



	if (vkQueuePresentKHR(logicalDevice.getQueues().presentationQueue, &presentInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to present");
	}
	currentFrame = (currentFrame + 1) % settings.maxBufferedImages;

	currentFrame = (currentFrame + 1) % settings.maxBufferedImages;
}

 void Renderer::mainLoop() {
	while (!glfwWindowShouldClose(glfwWindow)) {
		update();
		render();
	}
	vkDeviceWaitIdle(vkLogicalDevice);
}

 void Renderer::initCleanUp() {

}

 void Renderer::cleanUp() {

	for (size_t i = 0; i<meshes.size(); i++) {
		meshes[i].destroy();
	}

	fences.destroy(vkLogicalDevice, settings.maxBufferedImages);

	semaphores.destroy(vkLogicalDevice, settings.maxBufferedImages);

	commandPool.destroy(vkLogicalDevice);

	graphicsPipeline.destroy(vkLogicalDevice);

	renderPass.destroy(vkLogicalDevice);

	swapchain.destroy(logicalDevice.getLogicalDevice());

	logicalDevice.destory();

	window.destroy(vkInstance);

	instance.destroy(enableValidationLayers);

}
