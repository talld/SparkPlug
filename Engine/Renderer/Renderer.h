#ifndef SPARKPLUG_RENDERER_H
#define SPARKPLUG_RENDERER_H

#include "Allocator/Allocator.h"
#include "Window/Window.h"
#include "Instance/Instance.h"
#include "PhysicalDevice/PhysicalDevice.h"
#include "LogicalDevice/LogicalDevice.h"

#include <GLFW/glfw3.h>


class Renderer {

public:
    Renderer* create();
    Renderer* destroy();

private:
    Allocator allocator;
    Window window;
    Instance instance;
    PhysicalDevice physicalDevice;
    LogicalDevice logicalDevice;
};


#endif //SPARKPLUG_RENDERER_H
