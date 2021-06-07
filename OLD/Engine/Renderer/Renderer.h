#ifndef Renderer_H
#define Renderer_H

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>	
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <set>
#include <vector>
#include <array>
#include <optional>
#include <fstream>

#include <Renderer/Instance/Instance.h>
#include <Renderer/Window/Window.h>
#include <Renderer/PhysicalDevice/PhysicalDevice.h>
#include <Renderer/LogicalDevice/LogicalDevice.h>
#include <Renderer/Swapchain/Swapchain.h>
#include <Renderer/RenderPass/RenderPass.h>
#include <Renderer/DescriptorSet/DescriptorSet.h>
#include <Renderer/DescriptorSet/DescriptorPool.h>
#include <Renderer/GraphicsPipeline/GraphicsPipeline.h>
#include <Renderer/Command/CommandPool/CommandPool.h>
#include <Renderer/Command/CommandBuffer/CommandBuffer.h>
#include <Renderer/Sync/Fences/Fences.h>
#include <Renderer/Sync/Semaphores/Semaphores.h>
#include <Core/Object/Mesh/Mesh.h>
#include <Core/Object/Camera/Camera.h>

/*TODO: 
	Swapchain recreation
	Better error messages
	better errors
	online SPR-V compiler
	depth buffing
*/

class CommandBuffers;

class Instance;

class Window;



class Renderer
{

private:
	
	//Renderer objects
	GLFWwindow* glfwWindow;
	Window window;
	VkSurfaceKHR surface;

	VkInstance vkInstance;
	Instance instance;

	PhysicalDevice physicalDevice;
	VkPhysicalDevice vkPhysicalDevice;

	LogicalDevice logicalDevice;
	VkDevice vkLogicalDevice;

	VmaAllocator allocator;

	Swapchain swapchain;
	VkSwapchainKHR vkSwapchain;

	DescriptorSet descriptorSet;
	VkDescriptorSetLayout vkDescriptorSetLayout;

	DescriptorPool descriptorPool;
	VkDescriptorPool vkDescriptorPool;

	RenderPass renderPass;
	GraphicsPipeline graphicsPipeline;

	CommandPool commandPool;
	VkCommandPool graphicsCommandPool;
	CommandBuffers commandBuffer;

	Semaphores semaphores;
	Fences fences;

	//Renderer variables
	int currentFrame = 0;

	//Game Variables
	Camera* currentCamera;

	//Renderer functions
	void initVulkan();

	void initSettings();

	void update();

	void mainLoop();

	void render();

	void cleanUp();

public:

	struct Settings {
		int maxBufferedImages;
	} settings;


	void bootRender();

	void enterMainLoop();

	void terminate();

	//Game interface functions
	void createMesh(Mesh* mesh, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);

	void bindCamera(Camera* camera);

	void record(std::vector<Mesh>* meshes);

};
#endif