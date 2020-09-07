#pragma once
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
#include <Renderer/Swapchain/Swapchain.h>
#include <Renderer/GraphicsPipeline/GraphicsPipeline.h>
#include <Renderer/Command/CommandPool/CommandPool.h>
#include <Core/Object/Mesh/Mesh.h>

class Swapchain;

class CommandPool;

class GraphicsPipeline;

class Mesh;

class CommandBuffer
{
	
private:

	std::vector<VkCommandBuffer> commandBuffers;

public:

	void create(VkDevice vkLogicalDevice, Swapchain swapchain, VkCommandPool commandPool);

	void record(Swapchain swapchain, RenderPass renderPass, GraphicsPipeline graphicsPipeline, Mesh mesh);

	VkCommandBuffer* getCommandBufferP(int index);

	VkCommandBuffer getCommandBuffer(int index);

	std::vector<VkCommandBuffer> getCommandBuffers();
};

