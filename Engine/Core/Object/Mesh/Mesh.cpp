#include "Mesh.h"

VkBuffer Mesh::createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices) {

	this->data = allocateDynamicBufferTransferSpace(getUBOAllignment(sizeof(ubo), size));

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

	//all VMA_MEMORY_USAGE_CPU_ONLY memory is always coherent so no need to flush here

	//write only dst index buffer
	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, &vertexBuffer, &vertexAllocation);
	//transfer queue copy to gpu local
	copyBuffer(vkLogicalDevice, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);

	//we have written new data to the gpu so we need to flush the caches of our allocation
	vmaFlushAllocation(allocator, vertexAllocation, 0, bufferSize);

	//clean up staging buffer
	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

	return vertexBuffer;
}

VkBuffer Mesh::createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices) {

	VkDeviceSize bufferSize = sizeof(uint32_t) * indices->size();

	// temp SRC buffer
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, &stagingBuffer, &stagingAllocation);
	
	//cpu data move to src buffer
	void* data;
	vmaMapMemory(allocator, stagingAllocation, &data);
	memcpy(data, indices->data(), (size_t)bufferSize);
	vmaUnmapMemory(allocator, stagingAllocation);

	//all VMA_MEMORY_USAGE_CPU_ONLY memory is always coherent so no need to flush here

	//write only dst index buffer
	createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, &indexBuffer, &indexAllocation);
	//transfer queue copy to gpu local
	copyBuffer(vkLogicalDevice, transferQueue, transferCommandPool, stagingBuffer, indexBuffer, bufferSize);

	//we have written new data to the gpu so we need to flush the caches of our allocation
	vmaFlushAllocation(allocator, indexAllocation, 0, bufferSize);

	//clean up staging buffer	 
	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
	
	return indexBuffer;
}


void Mesh::create(PhysicalDevice physicalDevice, VkDevice vkLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, VmaAllocator allocator)
{
	this->transferQueue = transferQueue;
	this->transferCommandPool = transferCommandPool;
	this->vkPhysicalDevice = physicalDevice.getPhysicalDevice();
	this->vkLogicalDevice = vkLogicalDevice;
	this->allocator = allocator;
	this->ubo.model = glm::mat4(1.0f);
	this->size = physicalDevice.getMinUBOAllocation();
	create(vertices, indices);
}

//resue the mesh object without having to deconstruct and recreate it
void Mesh::create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices) {
	this->ubo.model = glm::mat4(1.0f);
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

void Mesh::setModel(glm::mat4 newModel) {
	ubo.model = newModel;
}

glm::mat4 Mesh::getModel() {
	return ubo.model;
}

void Mesh::destroy() {

	vertexCount = 0;
	indexCount = 0;

	vmaDestroyBuffer(allocator, vertexBuffer, vertexAllocation);

	vmaDestroyBuffer(allocator, indexBuffer, indexAllocation);

	freeDynamicBufferTransferSpace(data);
}
