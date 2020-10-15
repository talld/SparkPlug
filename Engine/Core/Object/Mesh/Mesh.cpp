#include "Mesh.h"

VkBuffer Mesh::createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices) {


	VkDeviceSize bufferSize = sizeof(Vertex) * vertices->size();

	// temp SRC buffer
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, &stagingBuffer, &stagingAllocation);


	//cpu data move to src buffer
	void* data;
	vmaMapMemory(allocator, stagingAllocation, &data);
	memcpy(data, vertices->data(), (size_t)bufferSize);
	vmaUnmapMemory(allocator, stagingAllocation);

	//write only dst index buffer
	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, &vertexBuffer, &vertexAllocation);
	//transfer queue copy to gpu local
	copyBuffer(vkLogicalDevice, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);
	//clean up staging buffer
	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

	return vertexBuffer;
}

VkBuffer Mesh::createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices) {

	VkDeviceSize bufferSize = sizeof(Vertex) * indices->size();

	// temp SRC buffer
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT| VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, &stagingBuffer, &stagingAllocation);


	//cpu data move to src buffer
	void* data;
	vmaMapMemory(allocator, stagingAllocation, &data);
	memcpy(data, indices->data(), (size_t)bufferSize);
	vmaUnmapMemory(allocator, stagingAllocation);

	//write only dst index buffer
	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, &indexBuffer, &indexAllocation);
	//transfer queue copy to gpu local
	copyBuffer(vkLogicalDevice, transferQueue, transferCommandPool, stagingBuffer, indexBuffer, bufferSize);
	//clean up staging buffer	 
	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

	return indexBuffer;
}


void Mesh::create(VkPhysicalDevice vkPhysicalDevice, VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, VmaAllocator allocator)
{
	this->transferQueue = transferQueue;
	this->transferCommandPool = transferCommandPool;
	this->vkPhysicalDevice = vkPhysicalDevice;
	this->vkLogicalDevice = vkLogicalDevice;
	this->allocator = allocator;
	create(vertices, indices);
}

//resue the mesh object without having to deconstruct and recreate it
void Mesh::create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices) {
	indexCount = indices->size();
	vertexCount = vertices->size();
	vertexBuffer = createVertexBuffer(transferQueue, transferCommandPool, vertices);
	indexBuffer = createIndexBuffer(transferQueue, transferCommandPool, indices);

}

int Mesh::getIndexCount() {
	return indexCount;
}

VkBuffer Mesh::getIndexBuffer() {
	return indexBuffer;
}

int Mesh::getVertexCount() {
	return vertexCount;
}

VkBuffer Mesh::getVetexBuffer() {
	return vertexBuffer;
}

void Mesh::destroy() {

	vertexCount = 0;
	indexCount = 0;

	vmaDestroyBuffer(allocator, vertexBuffer, vertexAllocation);

	vmaDestroyBuffer(allocator, indexBuffer, indexAllocation);
}
