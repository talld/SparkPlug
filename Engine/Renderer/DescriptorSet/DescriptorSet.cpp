#include "DescriptorSet.h"

VkDescriptorSetLayout DescriptorSet::createLayout(VkDevice lDevice) {

	this->lDevice = lDevice;

	VkDescriptorSetLayoutBinding MVPLayoutBinding{};
	MVPLayoutBinding.binding = 0;
	MVPLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	MVPLayoutBinding.descriptorCount = 1;
	MVPLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	MVPLayoutBinding.pImmutableSamplers = nullptr;


	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = &MVPLayoutBinding;
	descriptorSetLayoutCreateInfo.bindingCount = 1;

	VkResult result = vkCreateDescriptorSetLayout(lDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptorSet" + result);
	}

	return descriptorSetLayout;
}


void DescriptorSet::createUniformBuffers(VmaAllocator allocator, Swapchain swapchain) {

	this->allocator = allocator;

	//uniform buffer could be reworked to store just one matrix and passed premultiplied to the shader 
	VkDeviceSize bufferSize = sizeof(MVP);

	//one uniform buffer for each swapchain image to prevent altering a ubo while in use 
	uniformBuffers.resize(swapchain.getImageCount());
	uniformBufferAllocations.resize(swapchain.getImageCount());

	//countinue to use getImageCount as the result *should* be cached
	for (size_t i = 0; i < swapchain.getImageCount(); i++) {
		createBuffer(allocator, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &uniformBuffers[i], &uniformBufferAllocations[i]);
	}
}


void DescriptorSet::create(VkDescriptorPool descriptorPool) {

	descriptorSets.resize(uniformBuffers.size());

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(uniformBuffers.size(), descriptorSetLayout);

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(uniformBuffers.size());
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	VkResult result = vkAllocateDescriptorSets(lDevice, &descriptorSetAllocateInfo, descriptorSets.data());

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Faild to create Descriptor Sets " + result);
	}

	for (size_t i = 0; i < uniformBuffers.size(); i++) {

		VkDescriptorBufferInfo MVPBufferInfo{};
		MVPBufferInfo.buffer = uniformBuffers[i];
		MVPBufferInfo.offset = 0;					//im pretty sure this wont mess up vma allocations but keep an eye out
		MVPBufferInfo.range = sizeof(MVP);			//range of data (same as size)

		VkWriteDescriptorSet MVPSetWrite{};
		MVPSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		MVPSetWrite.dstSet = descriptorSets[i];
		MVPSetWrite.dstBinding = 0;
		MVPSetWrite.dstArrayElement = 0;								//index in array as we're not using data in an array 0 is fine
		MVPSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		MVPSetWrite.descriptorCount = 1;								//MVP is only a single descriptor SET despite having 3 descriptors (per image)
		MVPSetWrite.pBufferInfo = &MVPBufferInfo;

		vkUpdateDescriptorSets(lDevice, 1, &MVPSetWrite, 0, nullptr);	//this does not return a result type for some reason

	}
}

VkDescriptorSetLayout DescriptorSet::getDecriptorSetLayout() {
	return descriptorSetLayout;
}

VkDescriptorSet* DescriptorSet::getDescriptorSet(int i) {
	return &descriptorSets[i];
}

std::vector<VkBuffer> DescriptorSet::getUniformBuffers() {
	return uniformBuffers;
}

std::vector<VmaAllocation> DescriptorSet::getUniformBufferAllocations() {
	return uniformBufferAllocations;
}

void DescriptorSet::write(MVP mvp) {

	void* data;
	for (size_t i = 0; i < uniformBufferAllocations.size(); i++)
	{
		vmaMapMemory(allocator, uniformBufferAllocations[i], &data);
		memcpy(data, &mvp, sizeof(MVP));
		vmaUnmapMemory(allocator, uniformBufferAllocations[i]);
	}
}

void DescriptorSet::destroy() {
	
	vkDestroyDescriptorSetLayout(lDevice, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		vmaDestroyBuffer(allocator, uniformBuffers[i], uniformBufferAllocations[i]);
	}
}
