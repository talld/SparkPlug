#include "Renderer.h"

Renderer *Renderer::create() {

    glfwInit();

    window.create();

    instance.create(allocator);

    physicalDevice.create(allocator, instance);

    while(!window.closeRequested())
        glfwPollEvents();

    return this;
}

Renderer *Renderer::destroy() {

    instance.destroy(allocator);
    window.destroy(allocator,instance);

    return this;
}
