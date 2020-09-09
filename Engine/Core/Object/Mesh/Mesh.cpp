#include "Mesh.h"

inline VkBuffer Mesh::createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices) {


	VkDeviceSize bufferSize = sizeof(Vertex) * vertices->size();

	// temp SRC buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	createBuffer(vkPhysicalDevice, vkLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingMemory);


	//cpu data move to src buffer
	void* data; 
	vkMapMemory(vkLogicalDevice, stagingMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices->data(), (size_t)bufferSize);
	vkUnmapMemory(vkLogicalDevice, stagingMemory);

	//write only dst vertex buffer
	createBuffer(vkPhysicalDevice, vkLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT , &vertexBuffer, &vertexMemory);


	//TODO: device memory flushing here

	moveBuffer(vkLogicalDevice, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(vkLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(vkLogicalDevice, stagingMemory, nullptr);

	return vertexBuffer;
}


inline void Mesh::create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices)
{
	this->transferQueue = transferQueue;
	this->transferCommandPool = transferCommandPool;
	this->vkPhysicalDevice = vkPhysicalDevice;
	this->vkLogicalDevice = vkLogicalDevice;

	create(vertices);
}

inline void Mesh::create(std::vector<Vertex>* vertices) {
	vertexCount = vertices->size();
	this->vertexBuffer = createVertexBuffer(transferQueue, transferCommandPool, vertices);

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
