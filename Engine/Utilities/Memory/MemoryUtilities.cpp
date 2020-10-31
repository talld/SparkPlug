#include "MemoryUtilities.h"

VmaAllocator createAllocator(VkPhysicalDevice pDevice, VkDevice lDevice, VkInstance instance)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = pDevice;
	allocatorInfo.device = lDevice;
	allocatorInfo.instance = instance;
	VmaAllocator allocator;
	vmaCreateAllocator(&allocatorInfo, &allocator);
	return allocator;
}

void destroyAllocator(VmaAllocator allocator)
{
	vmaDestroyAllocator(allocator);
}

//get memory index where the type is compatible with the properties provided
uint32_t getMemoryTypeIndex(VkPhysicalDevice vkPhysicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags memoryPropertyFlags) {
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
		if ((allowedTypes & (1 << i)) && physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) {
			return i;
		}
	}
	return -1;
}

//buffer creation using AMD Vulkan memory allocator
void createBuffer(VmaAllocator allocator, VkDeviceSize bufferSize, VkBufferUsageFlags BufferUsageFlags, VmaMemoryUsage memoryUsage, VkBuffer* buffer, VmaAllocation* allocation)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = BufferUsageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocationCreateInfo{};
	allocationCreateInfo.usage = memoryUsage;

	vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, buffer, allocation, nullptr);
}

void copyBuffer(VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer src, VkBuffer dst, VkDeviceSize bufferSize) {

	vkResetFences(vkLogicalDevice, 1,	memUtilitesSettings.fences->getTransferFinishedP(0));

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

	vkQueueSubmit(transferQueue, 1, &submitInfo, memUtilitesSettings.fences->getTransferFinished(0));

	vkWaitForFences(vkLogicalDevice, 1, memUtilitesSettings.fences->getTransferFinishedP(0), VK_TRUE, UINT64_MAX);

	vkFreeCommandBuffers(vkLogicalDevice, transferCommandPool, 1, &transferCommandBuffer);
}

void setTransferSync(Fences* fences, int* currentFrame) {
	memUtilitesSettings.fences = fences;
	memUtilitesSettings.currentFrame = currentFrame;
}

VkDeviceSize getUBOAllignment(size_t structSize, VkDeviceSize minUBOAllocation) {
	return (structSize + minUBOAllocation - 1) & ~(minUBOAllocation - 1);
}

void* allocateDynamicBufferTransferSpace(VkDeviceSize UBOAllignment) {
	void* modelTransferSpace = (void*)_aligned_malloc(UBOAllignment * memUtilitesSettings.MAX_OBJECTS, UBOAllignment);	//really need something that isnt void pointer here
	return modelTransferSpace;
}

void freeDynamicBufferTransferSpace(void* data) {
	_aligned_free(data);
}

