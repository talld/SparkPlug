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

class Instnace;

class Window;

class PhysicalDevice;

class LogicalDeivce;

class Swapchain;

class RenderPass;


class GraphicsPipeline {
private:
	struct Pipeline {
		VkPipelineLayout layout;
		VkPipeline graphicsPipeline;
	};

	Pipeline pipeline;

	struct View {
		VkRect2D scissor{};
		VkViewport viewPort{};
	};

	View view;

	VkShaderModule createShadeModule(VkDevice logicalDevice,static std::vector<char>& code) {
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}

		return shaderModule;
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

public:

	void createGraphicsPipeline(VkDevice vkLogicalDevice, Swapchain swapchain, RenderPass renderPass) {

		auto vertexShaderCode = readFile("Shaders/vert.spv");
		auto fragmentShaderCode = readFile("Shaders/frag.spv");

		VkShaderModule vertexShaderModule = createShadeModule(vkLogicalDevice, vertexShaderCode);
		VkShaderModule fragmentShaderModule = createShadeModule(vkLogicalDevice, fragmentShaderCode);

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
		view.viewPort.width = (float)swapchain.getSwapchainDetails().extent2D.width;
		view.viewPort.height = (float)swapchain.getSwapchainDetails().extent2D.height;
		view.viewPort.minDepth = 0.0f;
		view.viewPort.maxDepth = 1.0f;

		viewPortCreateInfo.pViewports = &view.viewPort;

		viewPortCreateInfo.scissorCount = 1;

		view.scissor.offset = { 0,0 };
		view.scissor.extent = swapchain.getSwapchainDetails().extent2D;

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

		if (vkCreatePipelineLayout(vkLogicalDevice, &pipelineLayoutInfo, nullptr, &pipeline.layout) != VK_SUCCESS) {
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
		pipeLineCreateInfo.renderPass = renderPass.getRenderPasses().main;
		pipeLineCreateInfo.subpass = 0;
		pipeLineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipeLineCreateInfo.basePipelineIndex = -1;

		vkCreateGraphicsPipelines(vkLogicalDevice, VK_NULL_HANDLE, 1, &pipeLineCreateInfo, nullptr, &pipeline.graphicsPipeline);

		vkDestroyShaderModule(vkLogicalDevice, vertexShaderModule, nullptr);
		vkDestroyShaderModule(vkLogicalDevice, fragmentShaderModule, nullptr);
	}

	Pipeline getPipeline() {
		return pipeline;
	}

	VkRect2D* getViewScissor() {
		return &view.scissor;
	}

	VkViewport* getViewPort() {
		return &view.viewPort;
	}

	void destroy(VkDevice vkLogicalDevice) {
		vkDestroyPipeline(vkLogicalDevice, pipeline.graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(vkLogicalDevice, pipeline.layout, nullptr);
	}
};
