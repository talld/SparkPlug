#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>


//get memory index where the type is compatible with the properties provided
static uint32_t getMemoryTypeIndex(VkPhysicalDevice vkPhysicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags memoryPropertyFlags) {
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
		if ((allowedTypes & (1 << i)) && physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) {
			return i;
		}
	}
	return -1;
}


//creates buffer of provided size for provided usage in memory with provided properties
static void createBuffer(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags BufferUsageFlags, VkMemoryPropertyFlags MemoryProperties, VkBuffer* buffer, VkDeviceMemory* memory) {

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = BufferUsageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vkLogicalDevice, &bufferCreateInfo, nullptr, buffer)) {
		throw std::runtime_error("Failed to create Buffer");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkLogicalDevice, *buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = getMemoryTypeIndex(vkPhysicalDevice, memoryRequirements.memoryTypeBits,MemoryProperties); //TODO: device memory flushes

	if (vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, memory)) {
		throw std::runtime_error("Failed to allocate memory for vertex buffer");
	}

	vkBindBufferMemory(vkLogicalDevice, *buffer, *memory, 0);
}

static void moveBuffer(VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer src, VkBuffer dst, VkDeviceSize bufferSize) {

	VkCommandBuffer transferCommandBuffer;

	VkCommandBufferAllocateInfo transferCommandBufferAllocateInfo{};
	transferCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	transferCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	transferCommandBufferAllocateInfo.commandPool = transferCommandPool;
	transferCommandBufferAllocateInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(vkLogicalDevice, &transferCommandBufferAllocateInfo, &transferCommandBuffer);

	VkCommandBufferBeginInfo transferCommandBufferBeginInfo{};
	transferCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	transferCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //single use self destructive (might change later to perimanant (for computer shaders))

	vkBeginCommandBuffer(transferCommandBuffer, &transferCommandBufferBeginInfo);

	VkBufferCopy bufferCopy{};  //single copy from single region (could be optimaized to copy mutliple buffers at one)
	bufferCopy.srcOffset = 0;    
	bufferCopy.dstOffset = 0;
	bufferCopy.size = bufferSize;

	vkCmdCopyBuffer(transferCommandBuffer, src, dst, 1, &bufferCopy);
	
	vkEndCommandBuffer(transferCommandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pCommandBuffers = &transferCommandBuffer;
	submitInfo.commandBufferCount = 1;

	vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);

	vkQueueWaitIdle(transferQueue); //TODO: transfer semaphores and fences ASAP!

	vkFreeCommandBuffers(vkLogicalDevice, transferCommandPool, 1, &transferCommandBuffer);
}
