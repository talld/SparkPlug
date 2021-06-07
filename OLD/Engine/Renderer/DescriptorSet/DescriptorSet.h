#ifndef DescriptorSet_H
#define DescriptorSet_H


#include<vk_mem_alloc.h>
#include<vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

#include <Renderer/Swapchain/Swapchain.h>
#include <Utilities/Memory/MemoryUtilities.h>
#include <Core\Object\Camera\Camera.h>

struct ProjectionUBO;
struct MeshUBO;

class DescriptorSet

{
private:

	VkDevice lDevice;
	VkExtent2D extent;

	unsigned int swapchainImages;

	VkDescriptorSetLayout descriptorSetLayout;

	VmaAllocator allocator;

	std::vector<VkBuffer> VPUniformBuffers;
	std::vector<VmaAllocation> VPUniformBufferAllocations;

	void* modelTransferSpace;

	std::vector<VkBuffer> modelUniformBuffers;
	std::vector<VmaAllocation> modelUniformBufferAllocations;

	std::vector<VkDescriptorSet> descriptorSets;

public:
	void create(VkDescriptorPool descriptorPool);

	VkDescriptorSetLayout createLayout(VkDevice lDevice);

	void createUniformBuffers(VmaAllocator allocator, Swapchain swapchain);

	VkDescriptorSetLayout getDecriptorSetLayout();

	VkDescriptorSet* getDescriptorSet(int i);

	std::vector<VkBuffer> getVPUniformBuffers();

	std::vector<VmaAllocation> getVPUniformBufferAllocations();

	std::vector<VkBuffer> getModelUniformBuffers();

	std::vector<VmaAllocation> getModelUniformBufferAllocations();

	void emplaceMesh(MeshUBO meshUBO, int i);

	void write(ProjectionUBO mvp);

	void write(MeshUBO meshUBO);

	void destroy();
};

#endif