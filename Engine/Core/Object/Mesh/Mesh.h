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
#include <Renderer/GraphicsPipeline/GraphicsPipeline.h>
#include <Utilities/Memory/MemoryUtilities.h>


class Mesh
{
private:

	int vertexCount;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexMemory;


	int indexCount;
	VkBuffer indexBuffer;
	VkDeviceMemory indexMemory;

	VkPhysicalDevice vkPhysicalDevice;
	VkDevice vkLogicalDevice;
	VkQueue transferQueue;
	VkCommandPool transferCommandPool;

	VkBuffer createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices);

	VkBuffer createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices);

public:



	void create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);

	void create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);

	int getIndexCount();

	VkBuffer getIndexBuffer();

	int getVertexCount();

	VkBuffer getVetexBuffer();

	void destroy();
};

