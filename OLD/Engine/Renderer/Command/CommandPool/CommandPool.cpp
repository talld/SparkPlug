#include "CommandPool.h"

 void CommandPool::create(PhysicalDevice physicalDevice, VkDevice vkLogicalDevice) {

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex;

	if (vkCreateCommandPool(vkLogicalDevice, &commandPoolCreateInfo, nullptr, &commandPools.graphicsCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

 CommandPools CommandPool::getCommandPools() {
	return commandPools;
}

 void CommandPool::destroy(VkDevice vkLogicalDevice) {
	vkDestroyCommandPool(vkLogicalDevice, commandPools.graphicsCommandPool, nullptr);
}
