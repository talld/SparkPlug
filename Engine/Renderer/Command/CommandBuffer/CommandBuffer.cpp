#include "CommandBuffer.h"

void CommandBuffers::create(VkDevice vkLogicalDevice, Swapchain swapchain, VkCommandPool commandPool) {
	commandBuffers.resize(swapchain.getImageCount());

	VkCommandBufferAllocateInfo graphicsCommandBufferAllocateInfo{};
	graphicsCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	graphicsCommandBufferAllocateInfo.commandPool = commandPool;
	graphicsCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	graphicsCommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(swapchain.getSwapchainDetails().frameBuffers.size());

	if (vkAllocateCommandBuffers(vkLogicalDevice, &graphicsCommandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command buffers");
	}
}

void CommandBuffers::record(Swapchain swapchain, RenderPass renderPass, GraphicsPipeline graphicsPipeline, Camera* camera, std::vector<Mesh>* meshes) {

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

	for (size_t i = 0; i < commandBuffers.size(); i++) {

		renderPassBeginInfo.framebuffer = swapchain.getSwapchainDetails().frameBuffers[i];

		if (vkBeginCommandBuffer(commandBuffers[i], &graphicBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error(std::string("Failed to begin graphics command buffer record, interation :" + i));
		}

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getPipeline().graphicsPipeline);	
		for (size_t j = 0; j < meshes->size(); j++) {
			VkBuffer vertexBuffer[] = { meshes->at(j).getVetexBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffer, offsets);

			vkCmdBindIndexBuffer(commandBuffers[i], meshes->at(j).getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getPipeline().layout, 0, 1, camera->getDescriptorSet(i), 0, nullptr);

			vkCmdSetScissor(commandBuffers[i], 0, 1, graphicsPipeline.getViewScissor());

			vkCmdSetViewport(commandBuffers[i], 0, 1, graphicsPipeline.getViewPort());

			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(meshes->at(j).getIndexCount()), 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error(std::string("Failed to end graphics command buffer record, interation :" + i));
		}
	}
}

VkCommandBuffer* CommandBuffers::getCommandBufferP(int index) {
	return &commandBuffers[index];
}

VkCommandBuffer CommandBuffers::getCommandBuffer(int index) {
	return commandBuffers[index];
}

std::vector<VkCommandBuffer> CommandBuffers::getCommandBuffers() {
	return commandBuffers;
}

void CommandBuffers::destroy(VkDevice vkLogicalDevice, VkCommandPool vkCommandPool) {
	vkFreeCommandBuffers(vkLogicalDevice, vkCommandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}
