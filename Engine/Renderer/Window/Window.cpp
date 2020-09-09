#include "Window.h"

 void Window::create() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

}

 void Window::createSurface(VkInstance vkInstance) {

	if (glfwCreateWindowSurface(vkInstance, window, nullptr, &surface)) {
		throw std::runtime_error("Failed to create window surface");
	}

}

 void Window::update() {

}

 GLFWwindow* Window::getWindow() {
	return window;
}

 VkSurfaceKHR Window::getSurface() {
	return surface;
}

 void Window::destroy(VkInstance vkInstance) {
	vkDestroySurfaceKHR(vkInstance, surface, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

