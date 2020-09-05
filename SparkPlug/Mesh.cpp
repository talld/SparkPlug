#include "Mesh.h"

inline VkBuffer Mesh::createVertexBuffer(std::vector<Vertex>* vertices) {
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = sizeof(Vertex) * vertices->size();
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vkLogicalDevice, &bufferCreateInfo, nullptr, &vertexBuffer)) {
		throw std::runtime_error("Failed to create Buffer");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkLogicalDevice, vertexBuffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize  = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = getMemoryTypeIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); //TODO: device memory flushes

	if (vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, &vertexMemory)) {
		throw std::runtime_error("Failed to allocate memory for vertex buffer");
	}

	vkBindBufferMemory(vkLogicalDevice, vertexBuffer, vertexMemory, 0);

	void* data; 
	vkMapMemory(vkLogicalDevice, vertexMemory, 0, bufferCreateInfo.size, 0, &data);
	memcpy(data, vertices->data(), (size_t)bufferCreateInfo.size);
	vkUnmapMemory(vkLogicalDevice, vertexMemory);

	//TODO: flushing here

	return vertexBuffer;
}

inline uint32_t Mesh::getMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags memoryPropertyFlags) {
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
		if ((allowedTypes & (1 << i)) && physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) {
			return i;
		}
	}
	return -1;
}

inline void Mesh::create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, std::vector<Vertex>* vertices)
{
	this->vkPhysicalDevice = vkPhysicalDevice;
	this->vkLogicalDevice = vkLogicalDevice;
	create(vertices);
}

inline void Mesh::create(std::vector<Vertex>* vertices) {
	vertexCount = vertices->size();
	this->vertexBuffer = createVertexBuffer(vertices);

}

inline int Mesh::getVertexCount() {
	return vertexCount;
}

inline VkBuffer Mesh::getVetexBuffer() {
	return vertexBuffer;
}

inline void Mesh::destroy() {

	vkDestroyBuffer(vkLogicalDevice, vertexBuffer, nullptr);

	vkFreeMemory(vkLogicalDevice, vertexMemory, nullptr);
}
