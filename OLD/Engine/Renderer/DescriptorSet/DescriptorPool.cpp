#include "DescriptorPool.h"

VkDescriptorPool DescriptorPool::create(VkDevice lDevice, DescriptorSet descriptorSet) {

	this->lDevice = lDevice;

	VkDescriptorPoolSize VPPoolSize{};
	VPPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	VPPoolSize.descriptorCount = static_cast<uint32_t>(descriptorSet.getVPUniformBuffers().size());

	VkDescriptorPoolSize modelPoolSize{};
	modelPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelPoolSize.descriptorCount = static_cast<uint32_t>(descriptorSet.getModelUniformBuffers().size());


	VkDescriptorPoolSize descriptorPoolSizes[] = { VPPoolSize, modelPoolSize};

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(descriptorSet.getVPUniformBuffers().size());
	descriptorPoolCreateInfo.poolSizeCount = 2;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;


	VkResult result = vkCreateDescriptorPool(lDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Descriptor pool" + result);
	}

	return descriptorPool;
}

void DescriptorPool::destroy() {
	vkDestroyDescriptorPool(lDevice, descriptorPool, nullptr);
}
