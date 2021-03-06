#ifndef MemoryUtilites_H
#define MemoryUtilites_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <stdexcept>

#include <Renderer/LogicalDevice/LogicalDevice.h>
#include <Renderer/PhysicalDevice/PhysicalDevice.h>
#include <Renderer/Sync/Fences/Fences.h>
#include <Core/Object/Mesh/Mesh.h>

static struct MemUtilitesSettings {
	LogicalDevice logicalDevice;
	uint64_t minUBOAllocation;
	Fences* fences;
	int* currentFrame;
	const int MAX_OBJECTS = 20;
} memUtilitesSettings;

VmaAllocator createAllocator(VkPhysicalDevice pDevice, VkDevice lDevice, VkInstance instance);

void destroyAllocator(VmaAllocator allocator);

uint32_t getMemoryTypeIndex(VkPhysicalDevice vkPhysicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags memoryPropertyFlags);

void createBuffer(VmaAllocator allocator, VkDeviceSize bufferSize, VkBufferUsageFlags BufferUsageFlags, VmaMemoryUsage memoryUsage, VkBuffer* buffer, VmaAllocation* allocation);

void copyBuffer(VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer src, VkBuffer dst, VkDeviceSize bufferSize);

void setTransferSync(Fences* fences, int* currentFrame);


VkDeviceSize getUBOAllignment(size_t structSize, VkDeviceSize minUBOAllocation);

VkDeviceSize getUBOAllignment(size_t structSize);

unsigned int getUBOBlockSize(size_t structSize, VkDeviceSize minUBOAllocation);

void* allocateDynamicBufferTransferSpace(VkDeviceSize UBOAllignment);

void freeDynamicBufferTransferSpace(void* data);

#endif