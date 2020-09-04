#pragma once
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
#include "PhysicalDevice.h"

class PhysicalDevice;

class Instnace;

struct Queues {
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
};

class LogicalDevice {
private:

	Queues queues;

	VkDevice vkLogicalDevice;

public:
	void create(PhysicalDevice physicalDevice);

	VkDevice getLogicalDevice();

	Queues getQueues();

	void destory();

};