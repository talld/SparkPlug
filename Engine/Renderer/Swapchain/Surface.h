//
// Created by talldie on 01/07/2021.
//

#ifndef SPARKPLUG_SURFACE_H
#define SPARKPLUG_SURFACE_H

#include <vulkan/vulkan.h>

class Surface {

    VkSurfaceKHR vkSurface;

    Surface();

    Surface* create();

    Surface* destroy();

};


#endif //SPARKPLUG_SURFACE_H
