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



class Mesh
{
private:

	int vertexCount;
	VkBuffer vertexBuffer;

	VkDeviceMemory vertexMemory;

	VkPhysicalDevice vkPhysicalDevice;
	VkDevice vkLogicalDevice;

	VkBuffer createVertexBuffer(std::vector<Vertex>* vertices);

	uint32_t getMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags memoryPropertyFlags);

public:



	void create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, std::vector<Vertex>* vertices);

	void create(std::vector<Vertex>* vertices);

	int getVertexCount();

	VkBuffer getVetexBuffer();

	void destroy();
};

