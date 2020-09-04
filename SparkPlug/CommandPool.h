#pragma once
#define GLFW_INCLUDE_VULKAN
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

struct CommandPools {
	VkCommandPool graphicsCommandPool;
};

class CommandPool
{

private:

	CommandPools commandPools;

public:

	void create(PhysicalDevice physicalDevice, VkDevice vkLogicalDevice);

	CommandPools getCommandPools();

	void destroy(VkDevice vkLogicalDevice);
};

