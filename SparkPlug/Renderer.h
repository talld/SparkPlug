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

#include "Window.h"

#include "PhysicalDevice.h"

#include "LogicalDevice.h"

#include "Swapchain.h"

#include "RenderPass.h"

#include "GraphicsPipeline.h"


/*TODO: 
	Memory allocator
	Swapchain recreation
	Better error messages
	better errors
	online SPR-V compiler
	depth buffing
*/



class Renderer
{

public:

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



	void initVulkan() {
		instance.createInstance(enableValidationLayers);
		vkInstance = instance.getInstance();
		window.createSurface(vkInstance);
		surface = window.getSurface();
		physicalDevice.selectPhysicalDevice(vkInstance, surface, enableValidationLayers);
		vkPhysicalDevice = physicalDevice.getPhysicalDevice();
		logicalDevice.createLogicalDevice(physicalDevice);
		vkLogicalDevice = logicalDevice.getLogicalDevice();
		swapchain.createSwapchain(physicalDevice,logicalDevice, surface, window);
		vkSwapchain = swapchain.getSwapchain();
		renderPass.createRenderPass(vkLogicalDevice,swapchain);
		graphicsPipeline.createGraphicsPipeline(vkLogicalDevice,swapchain,renderPass);
		swapchain.createFrameBuffers(vkLogicalDevice,renderPass);
		createGrahpicsCommandPool();
		createCreateCommandBuffers();
		recordCommandBuffers();
		createSemaphores();
		createFences();
		initCleanUp();
	}



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
		int maxBufferedImages;
	};
	Settings settings;
	
	void initSettings() {
	settings.maxBufferedImages = 3;

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// vulkan functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Sync functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicSemaphores {
		VkSemaphore imageAvailable;
		VkSemaphore renderFinished;
	};

	std::vector <GraphicSemaphores> graphicSemaphores;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createSemaphores() {

		graphicSemaphores.resize(settings.maxBufferedImages);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			if (vkCreateSemaphore(vkLogicalDevice, &semaphoreCreateInfo, nullptr, &graphicSemaphores[i].imageAvailable) != VK_SUCCESS ||
				vkCreateSemaphore(vkLogicalDevice, &semaphoreCreateInfo, nullptr, &graphicSemaphores[i].renderFinished) != VK_SUCCESS) {

				throw std::runtime_error("failed to create graphic semaphores");
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SwapchainFences {
		VkFence imagesInFlight;
	};
	std::vector<SwapchainFences> swapchainFences;

	struct GraphicFences {
		VkFence renderFence;
	};
	std::vector<GraphicFences> graphicFences;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createFences() {
		graphicFences.resize(settings.maxBufferedImages);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			if (vkCreateFence(vkLogicalDevice, &fenceCreateInfo, nullptr, &graphicFences[i].renderFence) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create graphic fences");
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		commandBuffer functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void recordCommandBuffers() {

		VkCommandBufferBeginInfo graphicBufferBeginInfo{};
		graphicBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		graphicBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass.getRenderPasses().main;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapchain.getSwapchainDetails().extent2D;
		VkClearValue clearColor = { 0.65f, 0.6f, 0.4f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;
		//renderBeginPassInfo.framebuffer = swapchainFramebuffers[i];

		for (size_t i = 0; i < commandBuffers.graphicsCommandBuffers.size(); i++) {

			renderPassBeginInfo.framebuffer = swapchain.getSwapchainDetails().framebuffers[i];

			if (vkBeginCommandBuffer(commandBuffers.graphicsCommandBuffers[i], &graphicBufferBeginInfo) != VK_SUCCESS) {
				throw std::runtime_error(std::string("Failed to begin graphics command buffer record, interation :" + i));
			}
	
				vkCmdBeginRenderPass(commandBuffers.graphicsCommandBuffers[i],&renderPassBeginInfo,VK_SUBPASS_CONTENTS_INLINE);

					vkCmdBindPipeline(commandBuffers.graphicsCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getPipeline().graphicsPipeline);

					vkCmdSetScissor(commandBuffers.graphicsCommandBuffers[i], 0, 1, graphicsPipeline.getViewScissor());

					vkCmdSetViewport(commandBuffers.graphicsCommandBuffers[i], 0, 1, graphicsPipeline.getViewPort());

					vkCmdDraw(commandBuffers.graphicsCommandBuffers[i], 3, 1, 0, 0);

				vkCmdEndRenderPass(commandBuffers.graphicsCommandBuffers[i]);
			
			if (vkEndCommandBuffer(commandBuffers.graphicsCommandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error(std::string("Failed to end graphics command buffer record, interation :" + i));
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct CommandBuffers{
		std::vector<VkCommandBuffer> graphicsCommandBuffers;
	};

	CommandBuffers commandBuffers;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createCreateCommandBuffers() {
		commandBuffers.graphicsCommandBuffers.resize(swapchain.getSwapchainDetails().framebuffers.size());

		VkCommandBufferAllocateInfo graphicsCommandBufferAllocateInfo{};
		graphicsCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		graphicsCommandBufferAllocateInfo.commandPool = commandPools.graphicsCommandPool;
		graphicsCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		graphicsCommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(swapchain.getSwapchainDetails().framebuffers.size());

		if (vkAllocateCommandBuffers(vkLogicalDevice, &graphicsCommandBufferAllocateInfo, commandBuffers.graphicsCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create command buffers");
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		pool functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct CommandPools{
		VkCommandPool graphicsCommandPool;
	};

	CommandPools commandPools;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createGrahpicsCommandPool() {

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex;

		if (vkCreateCommandPool(vkLogicalDevice, &commandPoolCreateInfo, nullptr, &commandPools.graphicsCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Frame Buffer functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	

	


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		runtime functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	size_t currentFrame = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void update() {
		glfwPollEvents();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void render() {

		vkWaitForFences(vkLogicalDevice, 1, &graphicFences[currentFrame].renderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(vkLogicalDevice, 1, &graphicFences[currentFrame].renderFence);
		uint32_t imageIndex;
		vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, UINT64_MAX, graphicSemaphores[currentFrame].imageAvailable, VK_NULL_HANDLE, &imageIndex);

		VkSemaphore waitSemaphores[] = { graphicSemaphores[currentFrame].imageAvailable };
		VkSemaphore signalSemaphores[] = { graphicSemaphores[currentFrame].renderFinished };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers.graphicsCommandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(logicalDevice.getQueues().graphicsQueue, 1, &submitInfo, graphicFences[currentFrame].renderFence) != VK_SUCCESS) {
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void mainLoop() {
		while (!glfwWindowShouldClose(glfwWindow)) {
			update();
			render();
		}
		vkDeviceWaitIdle(vkLogicalDevice);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		cleanUp functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void initCleanUp() {

	}

	void cleanUp() {

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			vkDestroyFence(vkLogicalDevice, graphicFences[i].renderFence, nullptr);
		}

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			vkDestroySemaphore(vkLogicalDevice, graphicSemaphores[i].renderFinished, nullptr);
			vkDestroySemaphore(vkLogicalDevice, graphicSemaphores[i].imageAvailable, nullptr);
		}
		vkDestroyCommandPool(vkLogicalDevice, commandPools.graphicsCommandPool, nullptr);

		for (auto framebuffer : swapchain.getSwapchainDetails().framebuffers) {
			vkDestroyFramebuffer(vkLogicalDevice, framebuffer, nullptr);
		}


		graphicsPipeline.destroy(vkLogicalDevice);

		vkDestroyRenderPass(vkLogicalDevice, renderPass.getRenderPasses().main, nullptr);

		swapchain.destroy(logicalDevice.getLogicalDevice());

		logicalDevice.destory();

		window.destroy(vkInstance);

		instance.destroy(enableValidationLayers);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};