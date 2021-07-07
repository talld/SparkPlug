//
// Created by talldie on 06/07/2021.
//

#include "Renderpass.h"

Renderpass *Renderpass::create(const Allocator &allocator, const LogicalDevice &device, const Swapchain &swapchain) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.colorAttachmentCount = 1;


    std::array<VkSubpassDependency,2> dependencies{};

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    dependencies[0].dstSubpass = 0;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    dependencies[1].srcSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;


    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pAttachments = &colorAttachment;
    createInfo.attachmentCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.subpassCount = 1;
    createInfo.pDependencies = dependencies.data();
    createInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());


    VkResult res = vkCreateRenderPass(device.vkDevice, &createInfo, allocator.allocationCallbacksPtr, &vkRenderPass);

    if(res != VK_SUCCESS){
        allocator.throwError(res,"Failed to create RenderPass");
    }

    return this;
}

Renderpass *Renderpass::destroy(const Allocator &allocator, const LogicalDevice &device) {
    vkDestroyRenderPass(device.vkDevice, vkRenderPass, allocator.allocationCallbacksPtr);
    return this;
}
