#ifndef Camera_H
#define Camera_H


#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/DescriptorSet/DescriptorSet.h>
#include <Renderer/Swapchain/Swapchain.h>

class DescriptorSet;
class Swapchain;

struct ProjectionUBO {
	glm::mat4 projection;
	glm::mat4 view;
};

class Camera
{
	DescriptorSet* descriptorSet;
	VkExtent2D extent;
public:


	ProjectionUBO mvp;

	Camera() {

	}

	~Camera() {

	}

	void create(Swapchain swapchain, DescriptorSet* descriptorSet);

	void update();

	VkDescriptorSet* getDescriptorSet(int i);
};

#endif