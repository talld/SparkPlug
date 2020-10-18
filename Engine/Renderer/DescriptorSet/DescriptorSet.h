#ifndef DescriptorSet_H
#define DescriptorSet_H

#include<glm/glm.hpp>
#include<vk_mem_alloc.h>
#include<vulkan/vulkan.h>
#include <stdexcept>

class DescriptorSet
{
private:
	struct MVP {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
	} mvp;

	VkDescriptorSetLayout descriptorSetLayout;

	VkDevice device;

public:
	VkDescriptorSetLayout createDescriptorSetLayout(VkDevice lDevice);

	VkDescriptorSetLayout getDecriptorSetLayout();

	void destroy();
};

#endif