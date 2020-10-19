#include "DescriptorPool.h"

VkDescriptorPool DescriptorPool::create(VkDevice lDevice, DescriptorSet descriptorSet) {

	this->lDevice = lDevice;

	uint32_t size = static_cast<uint32_t>(descriptorSet.getUniformBuffers().size());

	VkDescriptorPoolSize descriptorPoolSize{};
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize.descriptorCount = size;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets = size;
	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;


	VkResult result = vkCreateDescriptorPool(lDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Descriptor pool" + result);
	}

	return descriptorPool;
}

void DescriptorPool::destroy() {
	vkDestroyDescriptorPool(lDevice, descriptorPool, nullptr);
}
