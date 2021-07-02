#ifndef Window_H
#define Window_H
#define GLFW_INCLUDE_VULKAN

#include "../Instance/Instance.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

class Instnace;

class Window {

private:

	VkSurfaceKHR surface;
	
public:

    GLFWwindow* glfwWindow;

	uint32_t WIDTH = 1024;
	uint32_t HEIGHT = 720;


    Window* create();

    void destroy(Allocator allocator, Instance instance);

    Window* update();

    Window* show();

    int closeRequested();

};

#endif