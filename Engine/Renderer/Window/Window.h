#ifndef Window_H
#define Window_H
#define GLFW_INCLUDE_VULKAN

#include "../Instance/Instance.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>

class Instnace;

class Window {

private:
	GLFWwindow* window;
	VkSurfaceKHR surface;
	
public:


	uint32_t WIDTH = 1024;
	uint32_t HEIGHT = 720;


    Window* create();

    void destroy(Allocator allocator, Instance instance);

    Window* update();

    int closeRequested();

};

#endif