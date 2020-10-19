#ifndef DescriptorSet_H
#define DescriptorSet_H

#include<glm/glm.hpp>
#include<vk_mem_alloc.h>
#include<vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

#include <Renderer/Swapchain/Swapchain.h>
#include <Utilities/Memory/MemoryUtilities.h>

class DescriptorSet
{
private:

	VkDevice lDevice;

	struct MVP {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
	} mvp;

	VkDescriptorSetLayout descriptorSetLayout;

	VmaAllocator allocator;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VmaAllocation> uniformBufferAllocations;

	std::vector<VkDescriptorSet> descriptorSets;

public:
	VkDescriptorSetLayout createLayout(VkDevice lDevice);

	void createUniformBuffers(VmaAllocator allocator, Swapchain swapchain);

	void create(VkDescriptorPool descriptorPool);

	VkDescriptorSetLayout getDecriptorSetLayout();

	VkDescriptorSet* getDescriptorSet(int i);

	std::vector<VkBuffer> getUniformBuffers();

	std::vector<VmaAllocation> getUniformBufferAllocations();

	void destroy();
};

#endif