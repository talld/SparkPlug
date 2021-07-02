#include "Window.h"

Window* Window::create() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwHideWindow(glfwWindow);
    return this;
}

void Window::destroy(Allocator allocator, Instance instance) {
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

Window* Window::update() {
     return this;
}

Window *Window::show() {
    glfwShowWindow(glfwWindow);
    return this;
}

int Window::closeRequested() {
    return glfwWindowShouldClose(glfwWindow);
}



