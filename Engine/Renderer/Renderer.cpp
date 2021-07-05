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

    window.show();

    while(!window.closeRequested())
        glfwPollEvents();

    return this;
}

Renderer *Renderer::destroy() {
    swapchainImages.destroy(allocator, logicalDevice);
    swapchain.destroy(allocator,logicalDevice);
    logicalDevice.destroy(allocator);
    surface.destroy(allocator, instance);
    instance.destroy(allocator);
    window.destroy(allocator,instance);

    return this;
}
