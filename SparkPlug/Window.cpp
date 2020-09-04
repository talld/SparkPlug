#include "Window.h"

inline void Window::init() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

inline void Window::createSurface(VkInstance vkInstance) {

	if (glfwCreateWindowSurface(vkInstance, window, nullptr, &surface)) {
		throw std::runtime_error("Failed to create window surface");
	}

}

inline void Window::update() {

}

inline GLFWwindow* Window::getWindow() {
	return window;
}

inline VkSurfaceKHR Window::getSurface() {
	return surface;
}

inline void Window::destroy(VkInstance vkInstance) {
	vkDestroySurfaceKHR(vkInstance, surface, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}
