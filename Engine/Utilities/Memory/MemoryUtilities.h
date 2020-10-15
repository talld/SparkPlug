#ifndef MemoryUtillites_H
#define MemoryUtillites_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <stdexcept>



VmaAllocator createAllocator(VkPhysicalDevice pDevice, VkDevice lDevice, VkInstance instance);

void destroyAllocator(VmaAllocator allocator);

uint32_t getMemoryTypeIndex(VkPhysicalDevice vkPhysicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags memoryPropertyFlags);

void createBuffer(VmaAllocator allocator, VkDeviceSize bufferSize, VkBufferUsageFlags BufferUsageFlags, VmaMemoryUsage memoryUsage, VkBuffer* buffer, VmaAllocation* allocation);

void copyBuffer(VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer src, VkBuffer dst, VkDeviceSize bufferSize);

#endif