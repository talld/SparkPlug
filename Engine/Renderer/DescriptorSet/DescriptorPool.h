#ifndef DescriptorPool_H
#define DescriptorPool_H

#include<vulkan/vulkan.h>

#include <Renderer/DescriptorSet/DescriptorSet.h>

class DescriptorPool
{
private:

	VkDevice lDevice;

	VkDescriptorPool descriptorPool;

public:

	VkDescriptorPool create(VkDevice lDevice, DescriptorSet descriptorSet);

	void destroy();
};

#endif