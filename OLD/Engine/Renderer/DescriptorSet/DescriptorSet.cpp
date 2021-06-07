#include "DescriptorSet.h"

VkDescriptorSetLayout DescriptorSet::createLayout(VkDevice lDevice) {

	this->lDevice = lDevice;

	VkDescriptorSetLayoutBinding VP_LayoutBinding{};
	VP_LayoutBinding.binding = 0;
	VP_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	VP_LayoutBinding.descriptorCount = 1;
	VP_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	VP_LayoutBinding.pImmutableSamplers = nullptr;


	VkDescriptorSetLayoutBinding model_LayoutBinding{};
	VP_LayoutBinding.binding = 1;
	VP_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	VP_LayoutBinding.descriptorCount = 1;
	VP_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	VP_LayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding layoutBindings[] = { VP_LayoutBinding, model_LayoutBinding };

	VkDescriptorSetLayoutCreateInfo VP_descriptorSetLayoutCreateInfo{};
	VP_descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	VP_descriptorSetLayoutCreateInfo.pBindings = layoutBindings;
	VP_descriptorSetLayoutCreateInfo.bindingCount = 2;

	VkResult result = vkCreateDescriptorSetLayout(lDevice, &VP_descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptorSet" + result);
	}

	return descriptorSetLayout;
}


void DescriptorSet::createUniformBuffers(VmaAllocator allocator, Swapchain swapchain) {

	this->allocator = allocator;
	this->swapchainImages = swapchain.getImageCount();

	//uniform buffer could be reworked to store just one matrix and passed pre-multiplied to the shader 
	VkDeviceSize VPBufferSize = sizeof(ProjectionUBO);
	
	VkDeviceSize modelBufferSize = getUBOAllignment(sizeof(MeshUBO) * memUtilitesSettings.MAX_OBJECTS);



	//one uniform buffer for each swapchain image to prevent altering a UBO while in use 
	VPUniformBuffers.resize(swapchainImages);
	VPUniformBufferAllocations.resize(swapchainImages);

	modelUniformBuffers.resize(swapchainImages);
	modelUniformBufferAllocations.resize(swapchainImages);


	//continue to use getImageCount as the result *should* be cached
	for (size_t i = 0; i < swapchainImages; i++) {
		createBuffer(allocator, VPBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &VPUniformBuffers[i], &VPUniformBufferAllocations[i]);

		createBuffer(allocator, modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &modelUniformBuffers[i], &modelUniformBufferAllocations[i]);
	}
}


void DescriptorSet::create(VkDescriptorPool descriptorPool) {

	descriptorSets.resize(swapchainImages);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(swapchainImages, descriptorSetLayout);

	this->modelTransferSpace = allocateDynamicBufferTransferSpace(getUBOAllignment(sizeof(MeshUBO)));


	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImages);
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	VkResult result = vkAllocateDescriptorSets(lDevice, &descriptorSetAllocateInfo, descriptorSets.data());

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Descriptor Sets " + result);
	}

	for (size_t i = 0; i < swapchainImages; i++) {
		
		VkDescriptorBufferInfo VPBufferInfo{};
		VPBufferInfo.buffer = VPUniformBuffers[i];
		VPBufferInfo.offset = 0;		
		VPBufferInfo.range = sizeof(ProjectionUBO);		    //range of data (same as size)

		VkWriteDescriptorSet VPSetWrite{};
		VPSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		VPSetWrite.dstSet = descriptorSets[i];
		VPSetWrite.dstBinding = 0;
		VPSetWrite.dstArrayElement = 0;								//index in array as we're not using data in an array 0 is fine
		VPSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		VPSetWrite.descriptorCount = 1;								//ProjectionUBO is only a single descriptor SET despite having 3 descriptors (per image)
		VPSetWrite.pBufferInfo = &VPBufferInfo;

		VkDescriptorBufferInfo modelBufferInfo{};
		modelBufferInfo.buffer = modelUniformBuffers[i];
		modelBufferInfo.offset = 0;
		modelBufferInfo.range = getUBOAllignment( sizeof(MeshUBO) );

		VkWriteDescriptorSet modelSetWrite{};
		modelSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		modelSetWrite.dstSet = descriptorSets[i];
		modelSetWrite.dstBinding = 1;
		modelSetWrite.dstArrayElement = 0;
		modelSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		modelSetWrite.descriptorCount = 1;
		modelSetWrite.pBufferInfo = &modelBufferInfo;


		VkWriteDescriptorSet descSetWrites[] = { VPSetWrite, modelSetWrite };

		vkUpdateDescriptorSets(lDevice, 2, descSetWrites, 0, nullptr);	//this does not return a result type for some reason

	}
}

VkDescriptorSetLayout DescriptorSet::getDecriptorSetLayout() {
	return descriptorSetLayout;
}

VkDescriptorSet* DescriptorSet::getDescriptorSet(int i) {
	return &descriptorSets[i];
}

std::vector<VkBuffer> DescriptorSet::getVPUniformBuffers() {
	return VPUniformBuffers;
}

std::vector<VmaAllocation> DescriptorSet::getVPUniformBufferAllocations() {
	return VPUniformBufferAllocations;
}

std::vector<VkBuffer> DescriptorSet::getModelUniformBuffers()
{
	return modelUniformBuffers;
}

std::vector<VmaAllocation> DescriptorSet::getModelUniformBufferAllocations()
{
	return modelUniformBufferAllocations;
}

void DescriptorSet::emplaceMesh(MeshUBO meshUBO, int i)
{
	MeshUBO* mesh = (MeshUBO*)((uint64_t)modelTransferSpace + (i * getUBOAllignment(sizeof(MeshUBO))));
	*mesh = meshUBO;

}

void DescriptorSet::write(ProjectionUBO VP) {

	void* data;
	for (size_t i = 0; i < VPUniformBufferAllocations.size(); i++)
	{
		vmaMapMemory(allocator, VPUniformBufferAllocations[i], &data);
		memcpy(data, &VP, sizeof(ProjectionUBO));
		vmaUnmapMemory(allocator, VPUniformBufferAllocations[i]);
	}
}




void DescriptorSet::write(MeshUBO meshUBO) {

	void* data;
	for (size_t i = 0; i < modelUniformBufferAllocations.size(); i++)
	{
		vmaMapMemory(allocator, modelUniformBufferAllocations[i], &data);
		memcpy(data, modelTransferSpace, memUtilitesSettings.minUBOAllocation);
		vmaUnmapMemory(allocator, modelUniformBufferAllocations[i]);
	}

}

void DescriptorSet::destroy() {

	vkDestroyDescriptorSetLayout(lDevice, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < VPUniformBuffers.size(); i++) {
		vmaDestroyBuffer(allocator, VPUniformBuffers[i], VPUniformBufferAllocations[i]);
	}
}
