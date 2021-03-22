#ifndef Window_H
#define Window_H
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
	VkInstance instance;
public:


	 uint32_t WIDTH = 1024;
	 uint32_t HEIGHT = 720;

	/// <summary>
	/// Createa glfw window
	/// </summary>
	void create();

	/// <summary>
	/// Creates a vulkan surfaceKHR
	/// </summary>
	/// <param name="vkInstance">The instance for which to create the instance</param>
	void createSurface(VkInstance& vkInstance);

	/// <summary>
	/// Update the Widow(currently does nothing)
	/// </summary>
	void update();

	GLFWwindow* getWindow();

	VkSurfaceKHR& getSurface();

	/// <summary>
	/// Destroys the window and its corrisponding surface
	/// </summary>
	void destroy();


};

#endif