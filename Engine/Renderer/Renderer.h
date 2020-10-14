#ifndef Renderer_H
#define Renderder_H

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#pragma once
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>
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

#include <Renderer/Instance/Instance.h>
#include <Renderer/Window/Window.h>
#include <Renderer/PhysicalDevice/PhysicalDevice.h>
#include <Renderer/LogicalDevice/LogicalDevice.h>
#include <Renderer/Swapchain/Swapchain.h>
#include <Renderer/RenderPass/RenderPass.h>
#include <Renderer/GraphicsPipeline/GraphicsPipeline.h>
#include <Renderer/Command/CommandPool/CommandPool.h>
#include <Renderer/Command/CommandBuffer/CommandBuffer.h>
#include <Renderer/Sync/Fences/Fences.h>
#include <Renderer/Sync/Semaphores/Semaphores.h>
#include <Core/Object/Mesh/Mesh.h>


/*TODO: 
	Memory allocator
	Swapchain recreation
	Better error messages
	better errors
	online SPR-V compiler
	depth buffing
*/

class CommandBuffer;

class Instance;

class Window;



class Renderer
{

public:



	struct Settings {
		int maxBufferedImages;
	};

	void bootRender();

	void enterMainLoop();

	void terminate();

private:


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

	VmaAllocator allocator;

	Swapchain swapchain;
	VkSwapchainKHR vkSwapchain;

	RenderPass renderPass;
	GraphicsPipeline graphicsPipeline;

	CommandPool commandPool;
	VkCommandPool graphicsCommandPool;
	CommandBuffer commandBuffer;

	Semaphores semaphores;
	Fences fences;

	std::vector<Mesh> meshes;

	int currentFrame = 0;

	void initVulkan();

	void initSettings();

	void update();

	void render();

	void mainLoop();

	void initCleanUp();

	void cleanUp();
};

#endif