#ifndef Semaphores_H
#define Semaphores_H


#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <set>
#include <vector>
#include <array>
#include <optional>
#include <fstream>


class Semaphores
{

private:

	std::vector <VkSemaphore> imagesAvailable;

	std::vector <VkSemaphore> rendersFinished;

	VkSemaphore transferFinished;

public:

	void create(VkDevice vkLogicalDevice, int maxBufferedImages);

	VkSemaphore getImagesAvailable(int index);

	VkSemaphore getRendersFinished(int index);

	void destroy(VkDevice vkLogicalDevice, int maxBufferedImages);

};

#endif