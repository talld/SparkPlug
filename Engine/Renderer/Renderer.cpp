#include "Renderer.h"

Renderer *Renderer::create() {

    glfwInit();

    window.create();
    instance.create(allocator);

    surface.create(allocator, instance, window);

    physicalDevice.create(allocator, instance, surface)
        ->checkSurfaceSupported(surface);

    surface.evaluateCapabilities(physicalDevice);

    logicalDevice.create(allocator, instance, physicalDevice, surface);

    swapchain.create(allocator, window, surface, logicalDevice);

    swapchainImages.create(allocator, logicalDevice, swapchain);

    Shader vertexShader;
    vertexShader.create(allocator, logicalDevice, "/home/talldie/GameDev/Engines/SparkPlug/Shaders/vert.spv");

    Shader fragmentShader;
    fragmentShader.create(allocator, logicalDevice, "/home/talldie/GameDev/Engines/SparkPlug/Shaders/frag.spv");

    graphicsPipeline.createLayout(allocator, logicalDevice)
            ->addShaderStage(vertexShader, VK_SHADER_STAGE_VERTEX_BIT)
            ->addShaderStage(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            ->create(allocator, logicalDevice, swapchain);

    fragmentShader.destroy(allocator, logicalDevice);
    vertexShader.destroy(allocator, logicalDevice);

    window.show();

    while(!window.closeRequested())
        glfwPollEvents();

    return this;
}

Renderer *Renderer::destroy() {
    graphicsPipeline.destroy(allocator, logicalDevice);
    swapchainImages.destroy(allocator, logicalDevice);
    swapchain.destroy(allocator,logicalDevice);
    logicalDevice.destroy(allocator);
    surface.destroy(allocator, instance);
    instance.destroy(allocator);
    window.destroy(allocator,instance);

    return this;
}
