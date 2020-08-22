#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <optional>

/*TODO: 
	Memory allocator
*/
class VkRender
{
public:
	void bootRender() {
		initWindow();
		initVulkan();
	}

	void enterMainLoop() {
		mainLoop();
	}

	void terminate() {
		cleanUp();
	}

private:

	void initVulkan() {
		createInstance();
		selectPhysicalDevice();
	}

	// validation layers

	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


	// vulkan functions
	int getDeviceScore(VkPhysicalDevice device) {

		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);q

		return -1;
	}


	struct QueueFamilyIndices {
		uint32_t graphicsFamilyIndex = -1;

		bool validate() {
			return (graphicsFamilyIndex != 1);
		}
	};

	QueueFamilyIndices indices;

	QueueFamilyIndices getQueueFamiles(VkPhysicalDevice pDevice) {
		QueueFamilyIndices deviceQueueFamilyIndices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				deviceQueueFamilyIndices.graphicsFamilyIndex = i;
				break;
			}

			i++;
		}
		return deviceQueueFamilyIndices;
	}

	struct Devices {
		VkPhysicalDevice pDevice;
		VkDevice lDevice;
	};

	Devices devices;

	void selectPhysicalDevice(){
		
		uint32_t physDeviceCount = 0; //physical device count
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, nullptr);

		if (physDeviceCount == 0){
			throw std::runtime_error("failed to locate a physical device");
		}

		std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, physDevices.data());

		std::vector<VkPhysicalDevice> compatibleDevices(physDeviceCount);
		for (const auto& device : physDevices) {
			QueueFamilyIndices testedIndices = getQueueFamiles(device);
			if (testedIndices.validate) {
				compatibleDevices.push_back(device);
			}
		}
	}

	VkInstance instance;

	void createInstance() {

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "App";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_0;
	
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//extension vector
		std::vector<const char*> instanceExtensions = std::vector<const char*>();

		//get extensions required by GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (uint16_t i = 0; i < glfwExtensionCount; i++) { // should have less then 64k extentions
			instanceExtensions.push_back(glfwExtensions[i]);
		}

		uint32_t instanceExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.enabledExtensionCount = instanceExtensionCount;
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();
		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan instance");
		}

		//get available extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		//debug messages
		if (enableValidationLayers) {
			std::cout << "Required extensions:\n";
			for (const auto& requiredExension : instanceExtensions) {
				std::cout << '\t' << requiredExension << '\n';
			}
			
			std::cout << "\navailable extensions:\n";
			for (const auto& extension : extensions) {
				if (enableValidationLayers) {
					std::cout << '\t' << extension.extensionName << '\n';
				}
			}
		}

		//extensions check
		for (const auto& requiredExension : instanceExtensions) {
			bool hasExtension = false;
			for (const auto& extension : extensions) {
				if (strcmp(extension.extensionName, requiredExension)) {
					hasExtension = true;
					break;
				}
			}

			if (!hasExtension) {
				throw std::runtime_error( std::string("Failed to get required extension: ") + requiredExension);
			}
		}
		
	}

	// window functions

	GLFWwindow* window;

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	// runtime functions

	void update() {
		glfwPollEvents();
	}

	void render() {

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			update();
			render();
		}
	}

	// termination functions

	void cleanUp() {
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

