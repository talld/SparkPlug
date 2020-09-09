#include "Instance.h"

 void Instance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

 VkResult Instance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

 void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

 void Instance::setupDebugMessenger(bool enableValidationLayers) {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

 VKAPI_ATTR VkBool32 VKAPI_CALL Instance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

 std::vector<const char*> Instance::getRequiredExensions(bool enableValidationLayers) {
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

 bool Instance::checkValidationLayerSupport() {
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

 void Instance::createInstance(bool enableValidationLayers) {

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
	std::vector<const char*> instanceExtensions = getRequiredExensions(enableValidationLayers);

	uint32_t instanceExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.enabledExtensionCount = instanceExtensionCount;
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}
	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
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
		for (const auto& requiredExtension : instanceExtensions) {
			std::cout << '\t' << requiredExtension << '\n';
		}

		std::cout << "\navailable extensions:\n";
		for (const auto& extension : extensions) {
			if (enableValidationLayers) {
				std::cout << '\t' << extension.extensionName << '\n';
			}
		}
	}

	//extensions check
	for (const auto& requiredExtension : instanceExtensions) {
		bool hasExtension = false;
		for (const auto& extension : extensions) {
			if (strcmp(extension.extensionName, requiredExtension) == 0) {
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension) {
			throw std::runtime_error(std::string("Failed to get required extension: ") + requiredExtension);
		}
	}
	if (enableValidationLayers) {
		setupDebugMessenger(enableValidationLayers);
	}

}

 VkInstance Instance::getInstance() {
	return vkInstance;
}

 void Instance::destroy(bool enableValidationLayers) {

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
	}

	vkDestroyInstance(vkInstance, nullptr);
}
