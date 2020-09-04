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
#include "Swapchain.h"

class Instnace;

class LogicalDeivce;

class Swapchain;

struct RenderPasses {
	VkRenderPass main;
};

class RenderPass {


private:
	RenderPasses renderPasses;

public:
	void create(VkDevice vkLogcialDevice, Swapchain swapchain);

	RenderPasses getRenderPasses();

	void destroy(VkDevice vkLogicalDevice);
};
