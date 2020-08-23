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
		setupDebugMessenger();
		selectPhysicalDevice();
		createLogicalDevice();
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

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
}
	}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

	VkDebugUtilsMessengerEXT debugMessenger;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
	}

	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	std::vector<const char*> getRequiredExensions() {
		//get extensions required by GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}
	

	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	// vulkan functions

	struct Queues {
		VkQueue graphicsQueue;
	};

	Queues queues;

	void createLogicalDevice() {

		float gPriority = 1.0f;

		VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueFamilyIndex = indices.graphicsFamilyIndex;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &gPriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		deviceCreateInfo.enabledExtensionCount = 0;
		deviceCreateInfo.ppEnabledExtensionNames = nullptr;
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		if (vkCreateDevice(devices.pDevice, &deviceCreateInfo, nullptr, &devices.lDevice)) {
			throw std::runtime_error("Failed to create logical device");
		}

		vkGetDeviceQueue(devices.lDevice, indices.graphicsFamilyIndex, 0, &queues.graphicsQueue);
	}

	int getDeviceScore(VkPhysicalDevice device) {

		int score = -1;

		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		//score device
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += (deviceProperties.limits.maxImageDimension2D + deviceProperties.limits.maxFragmentInputComponents + deviceProperties.limits.maxGeometryOutputVertices) * 2;
		}
		else {
			score += (deviceProperties.limits.maxImageDimension2D + deviceProperties.limits.maxFragmentInputComponents + deviceProperties.limits.maxGeometryOutputVertices);
		}

		return score;
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
			if (testedIndices.validate()) {
				compatibleDevices.push_back(device);
			}
		}

		int highest = -1;
		VkPhysicalDevice highestDevice = nullptr;
		for (const auto& device : compatibleDevices) {
			if (device) {
				int score = getDeviceScore(device);
				if (highest < score) {
					highest = score;
					highestDevice = device;
				}
			}
		}

		if (!highestDevice) {
			throw std::runtime_error("Failed to evaluate devices");
		}

		devices.pDevice = highestDevice;
		indices = getQueueFamiles(devices.pDevice);
	}

	VkInstance instance;

	void createInstance() {

		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("Failed to request Validation layers");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "App";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_1;
	
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//extension vector
		std::vector<const char*> instanceExtensions = getRequiredExensions();

		uint32_t instanceExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.enabledExtensionCount = instanceExtensionCount;
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) {
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		} else {
	       createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}
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
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroyDevice(devices.lDevice, nullptr);
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

