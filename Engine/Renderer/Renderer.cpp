#include "Renderer.h"

Renderer *Renderer::create() {

    glfwInit();

    window.create();
    instance.create(allocator);

    surface.create(allocator, instance, window);

    physicalDevice.create(allocator, instance, surface)
        ->checkSurfaceSupported(surface);

    surface.evaluateCapabilities(physicalDevice);

    logicalDevice.create(allocator, instance, physicalDevice);

    window.show();

    while(!window.closeRequested())
        glfwPollEvents();

    return this;
}

Renderer *Renderer::destroy() {

    logicalDevice.destroy(allocator);
    instance.destroy(allocator);
    window.destroy(allocator,instance);

    return this;
}
