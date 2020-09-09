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


class Instnace;

class Window {

private:
	GLFWwindow* window;
	VkSurfaceKHR surface;
	bool resized = false;

public:


	 uint32_t WIDTH = 1024;
	 uint32_t HEIGHT = 720;

	void create();

	void createSurface(VkInstance vkInstance);

	void update();

	GLFWwindow* getWindow();

	VkSurfaceKHR getSurface();

	void destroy(VkInstance vkInstance);


};
