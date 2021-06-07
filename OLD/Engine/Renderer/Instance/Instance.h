#ifndef Instance_H
#define Instance_H

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

class Window;

class Instance {

private:

	VkInstance vkInstance;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	VkDebugUtilsMessengerEXT debugMessenger;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void setupDebugMessenger(bool enableValidationLayers);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	/// <summary>
	/// Returns the extensions requred (eg: GLFW support)
	/// </summary>
	/// <param name="enableValidationLayers">If validations layers are enabled</param>
	/// <returns>Vector of const char* of the names of the extensions</returns>
	std::vector<const char*> getRequiredExensions(bool enableValidationLayers);


	/// <summary>
	/// Returns a bool baised on if validation layers specified in validationLayers are currently supported
	/// </summary>
	/// <returns>True if all are supported</returns>
	bool checkValidationLayerSupport();

public:

	/// <summary>
	/// Create a vulkan instance
	/// </summary>
	/// <param name="enableValidationLayers">If validations layers are enabled</param>
	void createInstance(bool enableValidationLayers);

	/// <summary>
	/// Returns the VkInstance if one a been created
	/// </summary>
	/// <returns>VkInstance	to create instance</returns>
	VkInstance getInstance();


	/// <summary>
	/// Destroys and deallocates the VkInstance and relivent componenets
	/// </summary>
	/// <param name="enableValidationLayers">If validations layers are enabled</param>
	void destroy(bool enableValidationLayers);

};

#endif