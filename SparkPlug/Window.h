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


class Instnace;

class PhysicalDevice;

class LogicalDeivce;

class Swapchain;

class RenderPass;

class GraphicsPipeline;


class Window {

private:
	GLFWwindow* window;

public:

	const uint32_t WIDTH = 1024;
	const uint32_t HEIGHT = 720;

	void init() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
	VkSurfaceKHR surface;

	void createSurface(VkInstance vkInstance) {

		if (glfwCreateWindowSurface(vkInstance, window, nullptr, &surface)) {
			throw std::runtime_error("Failed to create window surface");
		}

	}

	void update() {

	}

	GLFWwindow* getWindow() {
		return window;
	}

	VkSurfaceKHR getSurface() {
		return surface;
	}

	void destroy(VkInstance vkInstance) {
		vkDestroySurfaceKHR(vkInstance, surface, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}


};
