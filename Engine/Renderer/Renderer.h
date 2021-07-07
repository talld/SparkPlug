#ifndef SPARKPLUG_RENDERER_H
#define SPARKPLUG_RENDERER_H

#include "Allocator/Allocator.h"
#include "Window/Window.h"
#include "Instance/Instance.h"
#include "Surface/Surface.h"
#include "PhysicalDevice/PhysicalDevice.h"
#include "LogicalDevice/LogicalDevice.h"
#include "Swapchain/Swapchain.h"
#include "ImageView/ImageView.h"
#include "GraphicsPipeline/GraphicsPipeline.h"
#include "Shader/Shader.h"

#include <GLFW/glfw3.h>


class Renderer {

public:
    Renderer* create();
    Renderer* destroy();

private:
    Allocator allocator;
    Window window;
    Instance instance;
    Surface surface;
    PhysicalDevice physicalDevice;
    LogicalDevice logicalDevice;
    Swapchain swapchain;
    ImageView swapchainImages;
    GraphicsPipeline graphicsPipeline;
    Renderpass mainPass;
};


#endif //SPARKPLUG_RENDERER_H
