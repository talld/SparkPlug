#ifndef Camera_H
#define Camera_H

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <Renderer/DescriptorSet/DescriptorSet.h>

class Camera
{
private:
	DescriptorSet* descriptorSet;
public:
	Camera() {

	}
	
	~Camera() {

	}

	void create(DescriptorSet* descriptorSet) {
		this->descriptorSet = descriptorSet;
	}

	void update() {

	}

	VkDescriptorSet* getDescriptorSet(int i) {
		return descriptorSet->getDescriptorSet(i);
	}
};

#endif