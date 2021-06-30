#ifndef SPARKPLUG_RENDERER_H
#define SPARKPLUG_RENDERER_H

#include "Allocator/Allocator.h"
#include "Window/Window.h"
#include "Instance/Instance.h"
#include "PhysicalDevice/PhysicalDevice.h"

#include <GLFW/glfw3.h>


class Renderer {

public:
    Renderer* create();
    Renderer* destroy();

private:
    Window window;
    PhysicalDevice physicalDevice;
    Instance instance;
    Allocator allocator;

};


#endif //SPARKPLUG_RENDERER_H
