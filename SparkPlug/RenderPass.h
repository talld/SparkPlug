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

class GraphicsPipeline;

class RenderPass {

private:
	struct RenderPasses {
		VkRenderPass main;
	};
	RenderPasses renderPasses;

public:
	void createRenderPass(VkDevice vkLogcialDevice, Swapchain swapchain) {
		VkFormat swapchainFormat = swapchain.getSwapchainDetails().format.format;
		VkAttachmentDescription colorAttchment{};
		colorAttchment.format = swapchainFormat;
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

		if (vkCreateRenderPass(vkLogcialDevice, &renderPassCreateInfo, nullptr, &renderPasses.main) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Renderpass");
		}
	}

	RenderPasses getRenderPasses() {
		return renderPasses;
	}

};