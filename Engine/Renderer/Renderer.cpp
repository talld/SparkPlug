#include "Renderer.h"

Renderer *Renderer::create() {

    glfwInit();

    window.create();
    instance.create(allocator);
    physicalDevice.create(allocator, instance);
    logicalDevice.create(allocator, instance, physicalDevice);

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
