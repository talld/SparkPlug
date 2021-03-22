#include "DescriptorSet.h"

VkDescriptorSetLayout DescriptorSet::createLayout(VkDevice lDevice) {

	this->lDevice = lDevice;

	std::vector<VkDescriptorSetLayoutBinding> graphicsSetLayouts(0);

	VkDescriptorSetLayoutBinding MVPLayoutBinding{};
	MVPLayoutBinding.binding = 0;
	MVPLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	MVPLayoutBinding.descriptorCount = 1;
	MVPLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	MVPLayoutBinding.pImmutableSamplers = nullptr;

	graphicsSetLayouts.push_back(MVPLayoutBinding);

	std::vector<VkDescriptorSetLayoutBinding> computeSetLayouts(0);
	
	VkDescriptorSetLayoutBinding computeSRCBuffer{};
	MVPLayoutBinding.binding = 1;
	MVPLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	MVPLayoutBinding.descriptorCount = 1;
	MVPLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	MVPLayoutBinding.pImmutableSamplers = nullptr;

	computeSetLayouts.push_back(computeSRCBuffer);

	VkDescriptorSetLayoutBinding computeDSTBuffer{};
	MVPLayoutBinding.binding = 2;
	MVPLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	MVPLayoutBinding.descriptorCount = 1;
	MVPLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	MVPLayoutBinding.pImmutableSamplers = nullptr;

	computeSetLayouts.push_back(computeDSTBuffer);
	
	std::cout << "\n" << graphicsSetLayouts.size() << "\n";

	VkDescriptorSetLayoutCreateInfo graphicsDescriptorSetLayoutCreateInfo{};
	graphicsDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	graphicsDescriptorSetLayoutCreateInfo.pBindings = graphicsSetLayouts.data();
	graphicsDescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(graphicsSetLayouts.size());

	VkResult result = vkCreateDescriptorSetLayout(lDevice, &graphicsDescriptorSetLayoutCreateInfo, nullptr, &graphicsDescriptorSetLayout);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Graphics descriptorSet" + result);
	}

	VkDescriptorSetLayoutCreateInfo computeDescriptorSetLayoutCreateInfo{};
	computeDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	computeDescriptorSetLayoutCreateInfo.pBindings = computeSetLayouts.data();
	computeDescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(computeSetLayouts.size());

	result = vkCreateDescriptorSetLayout(lDevice, &computeDescriptorSetLayoutCreateInfo, nullptr, &computeDescriptorSetLayout);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Compute descriptorSet" + result);
	}

	return graphicsDescriptorSetLayout;
}


void DescriptorSet::createUniformBuffers(VmaAllocator allocator, Swapchain swapchain) {

	this->allocator = allocator;

	//uniform buffer could be reworked to store just one matrix and passed premultiplied to the shader 
	VkDeviceSize bufferSize = sizeof(projectionUBO);

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

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(uniformBuffers.size(), graphicsDescriptorSetLayout);

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
		MVPBufferInfo.offset = 0;							//im pretty sure this wont mess up vma allocations but keep an eye out
		MVPBufferInfo.range = sizeof(projectionUBO);		//range of data (same as size)

		VkWriteDescriptorSet MVPSetWrite{};
		MVPSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		MVPSetWrite.dstSet = descriptorSets[i];
		MVPSetWrite.dstBinding = 0;
		MVPSetWrite.dstArrayElement = 0;								//index in array as we're not using data in an array 0 is fine
		MVPSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		MVPSetWrite.descriptorCount = 1;								//projectionUBO is only a single descriptor SET despite having 3 descriptors (per image)
		MVPSetWrite.pBufferInfo = &MVPBufferInfo;

		vkUpdateDescriptorSets(lDevice, 1, &MVPSetWrite, 0, nullptr);	//this does not return a result type for some reason

	}
}

VkDescriptorSetLayout DescriptorSet::getDecriptorSetLayout() {
	return graphicsDescriptorSetLayout;
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

void DescriptorSet::write(projectionUBO mvp) {

	void* data;
	for (size_t i = 0; i < uniformBufferAllocations.size(); i++)
	{
		vmaMapMemory(allocator, uniformBufferAllocations[i], &data);
		memcpy(data, &mvp, sizeof(projectionUBO));
		vmaUnmapMemory(allocator, uniformBufferAllocations[i]);
	}
}

void DescriptorSet::destroy() {

	vkDestroyDescriptorSetLayout(lDevice, graphicsDescriptorSetLayout, nullptr);

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		vmaDestroyBuffer(allocator, uniformBuffers[i], uniformBufferAllocations[i]);
	}
}
