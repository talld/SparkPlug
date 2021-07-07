#include "GraphicsPipeline.h"
#include "../Renderpass/Renderpass.h"

GraphicsPipeline::GraphicsPipeline() {

    vkPipeline = {};
    vkPipelineLayout = {};

    shaders = std::vector<VkPipelineShaderStageCreateInfo>();
}

GraphicsPipeline * GraphicsPipeline::create(const Allocator &allocator, const LogicalDevice &device,
                                            const Swapchain &swapchain,
                                            const Renderpass &renderpass) {

    VkViewport viewPort;
    viewPort.x = 0;
    viewPort.y = 0;
    viewPort.width = (float) swapchain.extent.width;
    viewPort.height = (float) swapchain.extent.height;
    viewPort.minDepth = 0;
    viewPort.maxDepth = 1;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.extent;

    std::vector<VkDynamicState> dynamicStates = {


    };


    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.pVertexBindingDescriptions = 0;

    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;


    VkPipelineViewportStateCreateInfo viewPortCreateInfo{};
    viewPortCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    viewPortCreateInfo.pViewports = &viewPort;
    viewPortCreateInfo.viewportCount = 1;      //TODO: split screen support

    viewPortCreateInfo.pScissors = &scissor;
    viewPortCreateInfo.scissorCount = 1;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());


    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthBiasEnable = false;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth = 1;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


    VkPipelineColorBlendAttachmentState blendAttachmentCreateInfo{};
    blendAttachmentCreateInfo.colorWriteMask = VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT; //no alpha blending on purpose
    blendAttachmentCreateInfo.blendEnable = VK_TRUE;

    blendAttachmentCreateInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentCreateInfo.colorBlendOp = VK_BLEND_OP_MULTIPLY_EXT;

    blendAttachmentCreateInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachmentCreateInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentCreateInfo.alphaBlendOp = VK_BLEND_OP_SUBTRACT;


    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.pAttachments = &blendAttachmentCreateInfo;
    colorBlendCreateInfo.attachmentCount = 1;

    // TODO: Depth buffer stencil

    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    createInfo.layout = vkPipelineLayout;

    createInfo.pVertexInputState = &vertexInputCreateInfo;

    createInfo.pStages = shaders.data();
    createInfo.stageCount = shaders.size();

    createInfo.pVertexInputState = &vertexInputCreateInfo;
    createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    createInfo.pViewportState = &viewPortCreateInfo;
    createInfo.pDynamicState = &dynamicStateCreateInfo;
    createInfo.pRasterizationState = &rasterizerCreateInfo;
    createInfo.pMultisampleState = &multisampleCreateInfo;
    createInfo.pColorBlendState = &colorBlendCreateInfo;
    createInfo.renderPass = renderpass.vkRenderPass;

    VkResult res = vkCreateGraphicsPipelines(device.vkDevice, nullptr, 1, &createInfo, allocator.allocationCallbacksPtr, &vkPipeline);

    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create Pipeline");
    }

    return this;
}

GraphicsPipeline *GraphicsPipeline::destroy(const Allocator &allocator, const LogicalDevice &device) {
    vkDestroyPipeline(device.vkDevice, vkPipeline, allocator.allocationCallbacksPtr);
    return this;
}

GraphicsPipeline *GraphicsPipeline::createLayout(const Allocator &allocator, const LogicalDevice &device) {

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    createInfo.pSetLayouts = nullptr;
    createInfo.setLayoutCount = 0;

    createInfo.pPushConstantRanges = nullptr;
    createInfo.pushConstantRangeCount = 0;

    VkResult res = vkCreatePipelineLayout(device.vkDevice, &createInfo, allocator.allocationCallbacksPtr, &vkPipelineLayout);

    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create PipelineLayout");
    }

    return this;
}

GraphicsPipeline *GraphicsPipeline::addShaderStage(const Shader &shader, VkShaderStageFlagBits shaderStage) {

    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.module = shader.vkShaderModule;
    createInfo.stage = shaderStage;
    createInfo.pName = "main";

    shaders.push_back(createInfo);

    return this;
}