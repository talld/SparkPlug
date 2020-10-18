#include "DescriptorSet.h"

VkDescriptorSetLayout DescriptorSet::createDescriptorSetLayout(VkDevice lDevice) {

	this->device = lDevice;

	VkDescriptorSetLayoutBinding MVPLayoutBinding{};
	MVPLayoutBinding.binding = 0;
	MVPLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	MVPLayoutBinding.descriptorCount = 1;
	MVPLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	MVPLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = &MVPLayoutBinding;

	VkResult result = vkCreateDescriptorSetLayout(lDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptorSet" + result);
	}

	return descriptorSetLayout;
}

VkDescriptorSetLayout DescriptorSet::getDecriptorSetLayout() {
	return descriptorSetLayout;
}

void DescriptorSet::destroy() {
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}
