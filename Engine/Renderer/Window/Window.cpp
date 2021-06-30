#include "Window.h"

Window* Window::create() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    return this;
}

void Window::destroy(Allocator allocator, Instance instance) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

Window* Window::update() {
     return this;
}



int Window::closeRequested() {
    return glfwWindowShouldClose(window);
}

