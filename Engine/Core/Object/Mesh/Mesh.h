#ifndef Mesh_H
#define Mesh_H

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
#include <glm/glm.hpp>
#include <Renderer/GraphicsPipeline/GraphicsPipeline.h>
#include <Utilities/Memory/MemoryUtilities.h>


class Mesh
{
private:

	VmaAllocator allocator;

	int vertexCount;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexMemory;
	VmaAllocation vertexAllocation;

	int indexCount;
	VkBuffer indexBuffer;
	VmaAllocation indexAllocation;

	VkPhysicalDevice vkPhysicalDevice;
	VkDevice vkLogicalDevice;
	VkQueue transferQueue;
	VkCommandPool transferCommandPool;

	VkBuffer createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices);

	VkBuffer createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices);

public:

	void create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, VmaAllocator allocator);

	void create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);

	int getIndexCount();

	VkBuffer getIndexBuffer();

	int getVertexCount();

	VkBuffer getVetexBuffer();

	void destroy();
};

#endif