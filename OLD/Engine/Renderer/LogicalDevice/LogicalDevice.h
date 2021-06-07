#pragma once
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
#include <Renderer/PhysicalDevice/PhysicalDevice.h>

class PhysicalDevice;

class Instnace;

struct Queues {
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
};

class LogicalDevice {
private:
	   
	Queues queues;
	 
	VkDevice vkLogicalDevice;

public:
	/// <summary>
	/// creates VkLogicalDevice for the given physicalDevice
	/// </summary>
	/// <param name="physicalDevice">physicalDevice referance that the logicalDevice should be made for</param>
	void create(PhysicalDevice& physicalDevice);

	/// <summary>
	/// Returns VkDevice for the logicalDeivce
	/// </summary>
	/// <returns>VkDevice for the logicalDeivce</returns>
	VkDevice getLogicalDevice();

	/// <summary>
	/// Returns Queues struct for the logicalDeivce
	/// </summary>
	/// <returns>A Queues struct containing VkQueue objects for the logicalDevice</returns>
	Queues getQueues();


	/// <summary>
	/// Destroys the VkLogicalDevice
	/// </summary>
	void destory();

};