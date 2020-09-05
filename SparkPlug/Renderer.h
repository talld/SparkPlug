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

#include "Instance.h"
#include "Instance.cpp"

#include "Window.h"
#include "Window.cpp"

#include "PhysicalDevice.h"
#include "PhysicalDevice.cpp"

#include "LogicalDevice.h"
#include "LogicalDevice.cpp"

#include "Swapchain.h"
#include "Swapchain.cpp"

#include "RenderPass.h"
#include "RenderPass.cpp"


#include "GraphicsPipeline.h"
#include "GraphicsPipeline.cpp"

#include "CommandPool.h"
#include "CommandPool.cpp"

#include "CommandBuffer.h"
#include "CommandBuffer.cpp"

#include "Fences.h"
#include "Fences.cpp"

#include "Semaphores.h"
#include "Semaphores.cpp"

#include "Mesh.h"
#include "Mesh.cpp"

/*TODO: 
	Memory allocator
	Swapchain recreation
	Better error messages
	better errors
	online SPR-V compiler
	depth buffing
*/

class Instance;

class Window;



class Renderer
{

public:



	struct Settings {
		int maxBufferedImages;
	};

	void bootRender() {
		initSettings();
		window.init();
		glfwWindow = window.getWindow();
		initVulkan();
	}

	void enterMainLoop() {
		mainLoop();
	}

	void terminate() {
		cleanUp();
	}

private:

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	Settings settings;

	GLFWwindow* glfwWindow;
	Window window;
	VkSurfaceKHR surface;

	VkInstance vkInstance;
	Instance instance;

	PhysicalDevice physicalDevice;
	VkPhysicalDevice vkPhysicalDevice;

	LogicalDevice logicalDevice;
	VkDevice vkLogicalDevice;

	Swapchain swapchain;
	VkSwapchainKHR vkSwapchain;

	RenderPass renderPass;
	GraphicsPipeline graphicsPipeline;

	CommandPool commandPool;
	VkCommandPool graphicsCommandPool;
	CommandBuffer commandBuffer;

	Semaphores semaphores;
	Fences fences;

	Mesh mesh;

	int currentFrame = 0;

	void initVulkan() {
		instance.createInstance(enableValidationLayers);
		vkInstance = instance.getInstance();

		window.createSurface(vkInstance);
		surface = window.getSurface();

		physicalDevice.select(vkInstance, surface, enableValidationLayers);
		vkPhysicalDevice = physicalDevice.getPhysicalDevice();

		logicalDevice.create(physicalDevice);
		vkLogicalDevice = logicalDevice.getLogicalDevice();

		swapchain.create(physicalDevice,logicalDevice, surface, window);
		vkSwapchain = swapchain.getSwapchain();

		renderPass.create(vkLogicalDevice,swapchain);

		graphicsPipeline.create(vkLogicalDevice,swapchain,renderPass);

		swapchain.create(vkLogicalDevice,renderPass);

		std::vector<Vertex> meshVertices = {
	{{0.0, -0.4, 0.0}, {1.0f,0.0f,0.0f}},
	{{0.4, 0.4, 0.0}, {0.0f, 1.0f, 0.0f}},
	{{-0.4, 0.4, 0.0}, {0.0f, 0.0f, 1.0f}}
		};

		mesh.create(vkPhysicalDevice, vkLogicalDevice, &meshVertices);

		commandPool.create(physicalDevice, vkLogicalDevice);
		graphicsCommandPool = commandPool.getCommandPools().graphicsCommandPool;
		commandBuffer.create(vkLogicalDevice,swapchain,graphicsCommandPool);
		commandBuffer.record(swapchain,renderPass,graphicsPipeline, mesh);
		semaphores.create(vkLogicalDevice, settings.maxBufferedImages);
		fences.create(vkLogicalDevice, settings.maxBufferedImages);


		initCleanUp();
	}


	void initSettings() {
	settings.maxBufferedImages = 3;

	}

	void update() {
		glfwPollEvents();
	}
	void render() {

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

		if (vkQueueSubmit(logicalDevice.getQueues().graphicsQueue, 1, &submitInfo,fences.getGraphicsFence(currentFrame)) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
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
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(glfwWindow)) {
			update();
			render();
		}
		vkDeviceWaitIdle(vkLogicalDevice);
	}


	void initCleanUp() {

	}

	void cleanUp() {

		mesh.destroy();

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
};
