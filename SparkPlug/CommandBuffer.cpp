#include "CommandBuffer.h"

inline void CommandBuffer::create(VkDevice vkLogicalDevice, Swapchain swapchain, VkCommandPool commandPool) {
	commandBuffers.resize(swapchain.getSwapchainDetails().framebuffers.size());

	VkCommandBufferAllocateInfo graphicsCommandBufferAllocateInfo{};
	graphicsCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	graphicsCommandBufferAllocateInfo.commandPool = commandPool;
	graphicsCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	graphicsCommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(swapchain.getSwapchainDetails().framebuffers.size());

	if (vkAllocateCommandBuffers(vkLogicalDevice, &graphicsCommandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command buffers");
	}
}

inline void CommandBuffer::record(Swapchain swapchain, RenderPass renderPass, GraphicsPipeline graphicsPipeline) {

	VkCommandBufferBeginInfo graphicBufferBeginInfo{};
	graphicBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	graphicBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass.getRenderPasses().main;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = swapchain.getSwapchainDetails().extent2D;
	VkClearValue clearColor = { 0.65f, 0.6f, 0.4f, 1.0f };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;
	//renderBeginPassInfo.framebuffer = swapchainFramebuffers[i];

	for (size_t i = 0; i < commandBuffers.size(); i++) {

		renderPassBeginInfo.framebuffer = swapchain.getSwapchainDetails().framebuffers[i];

		if (vkBeginCommandBuffer(commandBuffers[i], &graphicBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error(std::string("Failed to begin graphics command buffer record, interation :" + i));
		}

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getPipeline().graphicsPipeline);

		vkCmdSetScissor(commandBuffers[i], 0, 1, graphicsPipeline.getViewScissor());

		vkCmdSetViewport(commandBuffers[i], 0, 1, graphicsPipeline.getViewPort());

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error(std::string("Failed to end graphics command buffer record, interation :" + i));
		}
	}
}

inline VkCommandBuffer* CommandBuffer::getCommandBufferP(int index) {
	return &commandBuffers[index];
}

inline VkCommandBuffer CommandBuffer::getCommandBuffer(int index) {
	return commandBuffers[index];
}

inline std::vector<VkCommandBuffer> CommandBuffer::getCommandBuffers() {
	return commandBuffers;
}
