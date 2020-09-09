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

inline VkBuffer Mesh::createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices) {

	VkDeviceSize bufferSize = sizeof(Vertex) * indices->size();

	// temp SRC buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	createBuffer(vkPhysicalDevice, vkLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingMemory);


	//cpu data move to src buffer
	void* data;
	vkMapMemory(vkLogicalDevice, stagingMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices->data(), (size_t)bufferSize);
	vkUnmapMemory(vkLogicalDevice, stagingMemory);

	//write only dst vertex buffer
	createBuffer(vkPhysicalDevice, vkLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexMemory);


	//TODO: device memory flushing here

	moveBuffer(vkLogicalDevice, transferQueue, transferCommandPool, stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(vkLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(vkLogicalDevice, stagingMemory, nullptr);

	return indexBuffer;
}


inline void Mesh::create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices)
{
	this->transferQueue = transferQueue;
	this->transferCommandPool = transferCommandPool;
	this->vkPhysicalDevice = vkPhysicalDevice;
	this->vkLogicalDevice = vkLogicalDevice;
	create(vertices, indices);
}

//resue the mesh object without having to deconstruct and recreate it
inline void Mesh::create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices) {
	indexCount = indices->size();
	vertexCount = vertices->size();
	vertexBuffer = createVertexBuffer(transferQueue, transferCommandPool, vertices);
	indexBuffer = createIndexBuffer(transferQueue, transferCommandPool, indices);

}

inline int Mesh::getIndexCount() {
	return indexCount;
}

inline VkBuffer Mesh::getIndexBuffer() {
	return indexBuffer;
}

inline int Mesh::getVertexCount() {
	return vertexCount;
}

inline VkBuffer Mesh::getVetexBuffer() {
	return vertexBuffer;
}

inline void Mesh::destroy() {

	vertexCount = 0;
	indexCount = 0;

	vkDestroyBuffer(vkLogicalDevice, vertexBuffer, nullptr);
	vkDestroyBuffer(vkLogicalDevice, indexBuffer, nullptr);

	vkFreeMemory(vkLogicalDevice, vertexMemory, nullptr);
	vkFreeMemory(vkLogicalDevice, indexMemory, nullptr);

}
