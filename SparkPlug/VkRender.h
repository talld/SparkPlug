#pragma once

#define GLFW_INCLUDE_VULKAN
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

/*TODO: 
	Memory allocator
	Swapchain recreation
	Better error messages
	better errors
	online SPR-V compiler
	depth buffing
*/



class VkRender
{

public:
	void bootRender() {
		initSettings();
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
		createSurface();
		selectPhysicalDevice();
		createLogicalDevice();
		createSwapchain();
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		createGrahpicsCommandPool();
		createCreateCommandBuffers();
		recordCommandBuffers();
		createSemaphores();
		createFences();
		initCleanUp();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Temporary file shaders system
	//
	//this will be replaced by a build into gslang compiler that compiles glsl into sprv on build
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
		int maxBufferedImages;
	};
	Settings settings;
	
	void initSettings() {
	settings.maxBufferedImages = 3;

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// validation layers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkDebugUtilsMessengerEXT debugMessenger;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
	throw std::runtime_error("failed to set up debug messenger!");
}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// vulkan functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Sync functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicSemaphores {
		VkSemaphore imageAvailable;
		VkSemaphore renderFinished;
	};

	std::vector <GraphicSemaphores> graphicSemaphores;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createSemaphores() {

		graphicSemaphores.resize(settings.maxBufferedImages);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			if (vkCreateSemaphore(devices.lDevice, &semaphoreCreateInfo, nullptr, &graphicSemaphores[i].imageAvailable) != VK_SUCCESS ||
				vkCreateSemaphore(devices.lDevice, &semaphoreCreateInfo, nullptr, &graphicSemaphores[i].renderFinished) != VK_SUCCESS) {

				throw std::runtime_error("failed to create graphic semaphores");
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SwapchainFences {
		VkFence imagesInFlight;
	};
	std::vector<SwapchainFences> swapchainFences;

	struct GraphicFences {
		VkFence renderFence;
	};
	std::vector<GraphicFences> graphicFences;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createFences() {
		graphicFences.resize(settings.maxBufferedImages);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			if (vkCreateFence(devices.lDevice, &fenceCreateInfo, nullptr, &graphicFences[i].renderFence) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create graphic fences");
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		commandBuffer functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void recordCommandBuffers() {

		VkCommandBufferBeginInfo graphicBufferBeginInfo{};
		graphicBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		graphicBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPasses.main;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapchainDetails.extent2D;
		VkClearValue clearColor = { 0.65f, 0.6f, 0.4f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;
		//renderBeginPassInfo.framebuffer = swapchainFramebuffers[i];

		for (size_t i = 0; i < commandBuffers.graphicsCommandBuffers.size(); i++) {

			renderPassBeginInfo.framebuffer = swapchainDetails.framebuffers[i];

			if (vkBeginCommandBuffer(commandBuffers.graphicsCommandBuffers[i], &graphicBufferBeginInfo) != VK_SUCCESS) {
				throw std::runtime_error(std::string("Failed to begin graphics command buffer record, interation :" + i));
			}
	
				vkCmdBeginRenderPass(commandBuffers.graphicsCommandBuffers[i],&renderPassBeginInfo,VK_SUBPASS_CONTENTS_INLINE);

					vkCmdBindPipeline(commandBuffers.graphicsCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

					vkCmdSetScissor(commandBuffers.graphicsCommandBuffers[i], 0, 1, &view.scissor);

					vkCmdSetViewport(commandBuffers.graphicsCommandBuffers[i], 0, 1, &view.viewPort);

					vkCmdDraw(commandBuffers.graphicsCommandBuffers[i], 3, 1, 0, 0);

				vkCmdEndRenderPass(commandBuffers.graphicsCommandBuffers[i]);
			
			if (vkEndCommandBuffer(commandBuffers.graphicsCommandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error(std::string("Failed to end graphics command buffer record, interation :" + i));
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct CommandBuffers{
		std::vector<VkCommandBuffer> graphicsCommandBuffers;
	};

	CommandBuffers commandBuffers;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createCreateCommandBuffers() {
		commandBuffers.graphicsCommandBuffers.resize(swapchainDetails.framebuffers.size());

		VkCommandBufferAllocateInfo graphicsCommandBufferAllocateInfo{};
		graphicsCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		graphicsCommandBufferAllocateInfo.commandPool = commandPools.graphicsCommandPool;
		graphicsCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		graphicsCommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.graphicsCommandBuffers.size());

		if (vkAllocateCommandBuffers(devices.lDevice, &graphicsCommandBufferAllocateInfo, commandBuffers.graphicsCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create command buffers");
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		pool functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct CommandPools{
		VkCommandPool graphicsCommandPool;
	};

	CommandPools commandPools;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createGrahpicsCommandPool() {

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = indices.graphicsFamilyIndex;

		if (vkCreateCommandPool(devices.lDevice, &commandPoolCreateInfo, nullptr, &commandPools.graphicsCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Frame Buffer functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createFrameBuffers() {
		swapchainDetails.framebuffers.resize(swapchainDetails.swapchainImages.size());

		for (size_t i = 0; i < swapchainDetails.framebuffers.size(); i++) {

			std::array<VkImageView, 1> attachments = {
				swapchainDetails.swapchainImages[i].imageView
			};

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPasses.main;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = swapchainDetails.extent2D.width;
			framebufferCreateInfo.height = swapchainDetails.extent2D.height;
			framebufferCreateInfo.layers = 1;


			if (vkCreateFramebuffer(devices.lDevice, &framebufferCreateInfo, nullptr, &swapchainDetails.framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create framebuffer");
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Shader Functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkShaderModule createShadeModule(static std::vector<char> &code) {
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(devices.lDevice, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}

		return shaderModule;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Pipeline Functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Pipeline{
	VkPipelineLayout layout;
	VkPipeline graphicsPipeline;
	};

	Pipeline pipeline;
	
	struct view{
		VkRect2D scissor{};
		VkViewport viewPort{};
	};

	view view;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createGraphicsPipeline() {

		auto vertexShaderCode = readFile("Shaders/vert.spv");
		auto fragmentShaderCode = readFile("Shaders/frag.spv");

		VkShaderModule vertexShaderModule = createShadeModule(vertexShaderCode);
		VkShaderModule fragmentShaderModule = createShadeModule(fragmentShaderCode);

		VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
		vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageCreateInfo.module = vertexShaderModule;
		vertexShaderStageCreateInfo.pName = "main"; //pointer to the entry point of shader code

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
		fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfo.module = fragmentShaderModule;
		fragShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageCreateInfo, fragShaderStageCreateInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
		vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
		vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
		vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
		vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
		inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewPortCreateInfo{};
		viewPortCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewPortCreateInfo.viewportCount = 1;
		
		view.viewPort.x = 0.0f;
		view.viewPort.y = 0.0f;
		view.viewPort.width = (float) swapchainDetails.extent2D.width;
		view.viewPort.height = (float)swapchainDetails.extent2D.height;
		view.viewPort.minDepth = 0.0f;
		view.viewPort.maxDepth = 1.0f;

		viewPortCreateInfo.pViewports = &view.viewPort;

		viewPortCreateInfo.scissorCount = 1;

		view.scissor.offset = { 0,0 };
		view.scissor.extent = swapchainDetails.extent2D;

		viewPortCreateInfo.pScissors = &view.scissor;
	
		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
		rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCreateInfo.depthClampEnable = VK_FALSE;
		rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerCreateInfo.lineWidth = 1.0f;
		rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizerCreateInfo.depthBiasClamp = 0.0f;
		rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
		multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
		multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingCreateInfo.minSampleShading = 1.0f; 
		multisamplingCreateInfo.pSampleMask = nullptr; 
		multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE; 
		multisamplingCreateInfo.alphaToOneEnable = VK_FALSE; 
		
		//TODO: depth buffing

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(devices.lDevice, &pipelineLayoutInfo, nullptr, &pipeline.layout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = 2;
		dynamicStateCreateInfo.pDynamicStates = dynamicStates;

		VkGraphicsPipelineCreateInfo pipeLineCreateInfo{};
		pipeLineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeLineCreateInfo.stageCount = 2;
		pipeLineCreateInfo.pStages = shaderStages;
		pipeLineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
		pipeLineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		pipeLineCreateInfo.pViewportState = &viewPortCreateInfo;
		pipeLineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipeLineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
		pipeLineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
		pipeLineCreateInfo.pColorBlendState = &colorBlending;
		pipeLineCreateInfo.pDepthStencilState = nullptr;
		pipeLineCreateInfo.layout = pipeline.layout;
		pipeLineCreateInfo.renderPass = renderPasses.main;
		pipeLineCreateInfo.subpass = 0;
		pipeLineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipeLineCreateInfo.basePipelineIndex = -1;
		
		vkCreateGraphicsPipelines(devices.lDevice, VK_NULL_HANDLE, 1, &pipeLineCreateInfo,nullptr,&pipeline.graphicsPipeline);

		vkDestroyShaderModule(devices.lDevice, vertexShaderModule, nullptr);
		vkDestroyShaderModule(devices.lDevice, fragmentShaderModule, nullptr);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Render passes and subpass functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct RenderPasses{
		VkRenderPass main;
	};
	RenderPasses renderPasses;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createRenderPass() {
		VkAttachmentDescription colorAttchment{};
		colorAttchment.format = swapchainDetails.format.format;
		colorAttchment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttchment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttchment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttchment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttchment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttchment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttchment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttchmentReference{};
		colorAttchmentReference.attachment = 0; //index in subpass	
		colorAttchmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttchmentReference;

		//create transfer windows between dependent points in the renderpass (in(VK_IMAGE_LAYOUT_UNDEFINED) - subpass1(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) - out(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR))
		std::array<VkSubpassDependency, 2> subpassDependencies;
		subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // start of window (stage)
		subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT; // start of window (point in stage)
		subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL; //in
		subpassDependencies[0].dstSubpass = 0;//subpass1
		subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //end of window
		subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // end of window (point in stage)
		subpassDependencies[0].dependencyFlags = 0;

		subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; 
		subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[1].srcSubpass = 0; //subpass1
		subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; //out
		subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[1].dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttchment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (vkCreateRenderPass(devices.lDevice,&renderPassCreateInfo,nullptr,&renderPasses.main) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Renderpass");
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Image functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(devices.lDevice, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Image view");
		}
		return imageView;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Swapchain functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
		if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
			return surfaceCapabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtent = { WIDTH, HEIGHT };

			actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkPresentModeKHR selectSwapchainPresentMode(std::vector<VkPresentModeKHR> presentModes) {
		for (const auto& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return presentMode;
			}
		}
		//fifo mode always has to be available for vulkan compatibillity anyway so good backup
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkFormat selectSwapchainFormat(VkSurfaceFormatKHR format, std::vector<VkSurfaceFormatKHR> availableFormats) {
		
		VkFormat chosenFormat = availableFormats[0].format;
		
		switch (format.colorSpace)
		{
		case(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR):
			for (const auto& availableFormat: availableFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB) {
					chosenFormat = VK_FORMAT_B8G8R8A8_SRGB;
				}
			}
			break;
		default:
			break;
		}

		return chosenFormat;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkColorSpaceKHR selectSwapchainColorSpace(VkSurfaceFormatKHR format, std::vector<VkSurfaceFormatKHR> availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			}
		}
		return availableFormats[0].colorSpace;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkSurfaceFormatKHR selectSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats) {
		
		VkSurfaceFormatKHR format{};

		format.colorSpace = selectSwapchainColorSpace(format, availableFormats);
		format.format = selectSwapchainFormat(format, availableFormats);

		return format;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SwapChainSupportDetails getSurfaceSwapchainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails supportDetails;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails.capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount > 0) {
			supportDetails.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, supportDetails.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount > 0) {
			supportDetails.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, supportDetails.presentModes.data());
		}

		return supportDetails;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SwapchainImage {
		VkImage image;
		VkImageView imageView;
	};
	
	struct SwapchainDetails {
		VkExtent2D extent2D;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR presentMode;
		std::vector<SwapchainImage> swapchainImages;
		std::vector<VkFramebuffer> framebuffers;
	};
	SwapchainDetails swapchainDetails;

	VkSwapchainKHR swapchain;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createSwapchain() {
		SwapChainSupportDetails swapChainSupport = getSurfaceSwapchainSupport(devices.pDevice);

		VkSurfaceFormatKHR format = selectSwapchainSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = selectSwapchainPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = selectSwapExtent(swapChainSupport.capabilities);
		uint32_t minImageCount;
		if (swapChainSupport.capabilities.maxImageCount != 0) {
			minImageCount = std::min(swapChainSupport.capabilities.minImageCount + 1, swapChainSupport.capabilities.maxImageCount);
		}
		else {
			minImageCount = swapChainSupport.capabilities.minImageCount + 1;
		}
		VkSwapchainCreateInfoKHR swapChainCreateInfo{};

		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = surface;
		swapChainCreateInfo.minImageCount = minImageCount;
		swapChainCreateInfo.imageFormat = format.format;
		swapChainCreateInfo.imageColorSpace = format.colorSpace;
		swapChainCreateInfo.imageExtent = extent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.clipped = VK_TRUE;

		if (indices.graphicsFamilyIndex != indices.presentationFamilyIndex) {
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.graphicsFamilyIndex), static_cast<uint32_t>(indices.presentationFamilyIndex) };
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
			swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		if (vkCreateSwapchainKHR(devices.lDevice, &swapChainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create swapchain");
		}

		swapchainDetails.extent2D = extent;
		swapchainDetails.format = format;
		swapchainDetails.presentMode = presentMode;

		uint32_t swapChainImageCount = 0;
		vkGetSwapchainImagesKHR(devices.lDevice, swapchain, &swapChainImageCount, nullptr);
		std::vector<VkImage> images(swapChainImageCount);
		vkGetSwapchainImagesKHR(devices.lDevice, swapchain, &swapChainImageCount, images.data());

		for (VkImage image : images) {
			SwapchainImage swapchainImage{};
			swapchainImage.image = image;
			swapchainImage.imageView = createImageView(image, format.format, VK_IMAGE_ASPECT_COLOR_BIT);
			swapchainDetails.swapchainImages.push_back(swapchainImage);
		}

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		logical device + queue functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Queues {
		VkQueue graphicsQueue;
		VkQueue presentationQueue;
	};

	Queues queues;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const std::vector<const char*> physicalDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	void createLogicalDevice() {

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		std::set<int> uniqueQueueFamilies = { indices.graphicsFamilyIndex, indices.presentationFamilyIndex };

		float qPriority = 1.0f;
		for (int queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &qPriority;

			deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = physicalDeviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		if (vkCreateDevice(devices.pDevice, &deviceCreateInfo, nullptr, &devices.lDevice) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device");
		}

		vkGetDeviceQueue(devices.lDevice, indices.graphicsFamilyIndex, 0, &queues.graphicsQueue);
		vkGetDeviceQueue(devices.lDevice, indices.presentationFamilyIndex, 0, &queues.presentationQueue);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct QueueFamilyIndices {
		int graphicsFamilyIndex = -1;
		int presentationFamilyIndex = -1;

		bool validate() {
			return (graphicsFamilyIndex != -1 && presentationFamilyIndex != -1);
		}
	};

	QueueFamilyIndices indices;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		i = 0;
		for (const auto& queueFamily : queueFamilies) {
			VkBool32 surfaceSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, surface, &surfaceSupported);
			if (surfaceSupported) {
				deviceQueueFamilyIndices.presentationFamilyIndex = i;
				break;
			}

			i++;
		}
		return deviceQueueFamilyIndices;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Physical device functions 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Devices {
		VkPhysicalDevice pDevice;
		VkDevice lDevice;
	};

	Devices devices;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void selectPhysicalDevice() {

		uint32_t physDeviceCount = 0; //physical device count
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, nullptr);

		if (physDeviceCount == 0) {
			throw std::runtime_error("failed to locate a physical device");
		}

		std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, physDevices.data());

		std::vector<VkPhysicalDevice> compatibleDevices(physDeviceCount);

		bool foundCompatible = false;
		for (const auto& device : physDevices) {
			if (checkPhysicalDeviceCompatible(device)) {
				compatibleDevices.push_back(device);
				foundCompatible = true;
			}
		}

		if (foundCompatible == false) {
			throw std::runtime_error("Failed to find compatible device");
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

		if (highestDevice == nullptr || highest == -1) {
			throw std::runtime_error("Failed to evaluate devices");
		}

		devices.pDevice = highestDevice;
		indices = getQueueFamiles(devices.pDevice);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Instance functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkInstance instance;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
			createInfo.ppEnabledLayerNames = validationLayers.data();
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		}
		else {
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

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Checker functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool checkDeviceSwapChain(VkPhysicalDevice device) {
		SwapChainSupportDetails  swapChainSupport= getSurfaceSwapchainSupport(device);
		return !swapChainSupport.presentModes.empty() && !swapChainSupport.formats.empty();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool checkPhysicalDeviceExtemsions(VkPhysicalDevice device) {
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		if (extensionCount == 0) {
			return false;
		}

		std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.data());

		for (const auto& requiredExtension : physicalDeviceExtensions) {
			bool hasExtension = false;	
			for (const auto& deviceExtension : deviceExtensions) {
				if (strcmp(requiredExtension, deviceExtension.extensionName) == 0) {
					hasExtension = true;
					break;
				}
			}
				if (!hasExtension) {
					if (enableValidationLayers) {
						VkPhysicalDeviceProperties deviceProperites{};
						vkGetPhysicalDeviceProperties(device, &deviceProperites);
						std::cout << '\n' << deviceProperites.deviceName << ' ' <<deviceProperites.deviceID << '\t' <<"failed extension check: " << requiredExtension <<'\n';

					}
					return false;
				}
			}
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool checkPhysicalDeviceQueues(VkPhysicalDevice device) {

		QueueFamilyIndices testedIndices = getQueueFamiles(device);
		if (testedIndices.validate()) {
			return true;
		}
		return false;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		Group checker functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool checkPhysicalDeviceCompatible(VkPhysicalDevice device) {
		return (checkPhysicalDeviceExtemsions(device) && checkPhysicalDeviceQueues(device) && checkDeviceSwapChain(device));
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//		window functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkSurfaceKHR surface;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void createSurface() {

		if (glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
			throw std::runtime_error("Failed to create window surface");
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GLFWwindow* window;

	const uint32_t WIDTH = 1024;
	const uint32_t HEIGHT = 720;
	size_t currentFrame = 0;

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		runtime functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void update() {
		glfwPollEvents();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void render() {

		vkWaitForFences(devices.lDevice, 1, &graphicFences[currentFrame].renderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(devices.lDevice, 1, &graphicFences[currentFrame].renderFence);
		uint32_t imageIndex;
		vkAcquireNextImageKHR(devices.lDevice, swapchain, UINT64_MAX, graphicSemaphores[currentFrame].imageAvailable, VK_NULL_HANDLE, &imageIndex);

		VkSemaphore waitSemaphores[] = { graphicSemaphores[currentFrame].imageAvailable };
		VkSemaphore signalSemaphores[] = { graphicSemaphores[currentFrame].renderFinished };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers.graphicsCommandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, graphicFences[currentFrame].renderFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkSwapchainKHR swapchains[] = { swapchain };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional

		if (vkQueuePresentKHR(queues.presentationQueue, &presentInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to present");
		}
		currentFrame = (currentFrame + 1) % settings.maxBufferedImages;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			update();
			render();
		}
		vkDeviceWaitIdle(devices.lDevice);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		cleanUp functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void initCleanUp() {

	}

	void cleanUp() {

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			vkDestroyFence(devices.lDevice, graphicFences[i].renderFence, nullptr);
		}

		for (size_t i = 0; i < settings.maxBufferedImages; i++) {
			vkDestroySemaphore(devices.lDevice, graphicSemaphores[i].renderFinished, nullptr);
			vkDestroySemaphore(devices.lDevice, graphicSemaphores[i].imageAvailable, nullptr);
		}
		vkDestroyCommandPool(devices.lDevice, commandPools.graphicsCommandPool, nullptr);

		for (auto framebuffer : swapchainDetails.framebuffers) {
			vkDestroyFramebuffer(devices.lDevice, framebuffer, nullptr);
		}


		vkDestroyPipeline(devices.lDevice, pipeline.graphicsPipeline, nullptr);

		vkDestroyPipelineLayout(devices.lDevice,pipeline.layout, nullptr);

		vkDestroyRenderPass(devices.lDevice, renderPasses.main, nullptr);

		for (auto& image : swapchainDetails.swapchainImages) {
			vkDestroyImageView(devices.lDevice, image.imageView, nullptr);
		}
		
		vkDestroySwapchainKHR(devices.lDevice, swapchain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroyDevice(devices.lDevice, nullptr);
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

