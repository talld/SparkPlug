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
#include <glm/glm.hpp>
#include <Renderer/Swapchain/Swapchain.h>


class Instnace;

class PhysicalDevice;

class LogicalDeivce;

class Swapchain;

class RenderPass;

struct Pipeline {
	VkPipelineLayout layout;
	VkPipeline graphicsPipeline;
};

struct View {
	VkRect2D scissor{};
	VkViewport viewPort{};
};


struct Vertex {
	glm::vec3 position; //xyz vertex data
	glm::vec3 color;
};
class GraphicsPipeline {

private:

	Pipeline pipeline;

	View view;

	VkShaderModule createShadeModule(VkDevice logicalDevice, static std::vector<char>& code);

	std::vector<char> readFile(const std::string& filename);

public:

	void create(VkDevice vkLogicalDevice, Swapchain swapchain, RenderPass renderPass);

	Pipeline getPipeline();

	VkRect2D* getViewScissor();

	VkViewport* getViewPort();

	void destroy(VkDevice vkLogicalDevice);
};