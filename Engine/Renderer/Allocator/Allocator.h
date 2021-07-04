//
// Created by talldie on 29/06/2021.
//

#ifndef SPARKPLUG_ALLOCATOR_H
#define SPARKPLUG_ALLOCATOR_H

#include <vulkan/vulkan.h>
#include "stdexcept"

class Allocator {

public:
    //using a pointer here so i can mess around with sys mem later down the line without having to find and replace everything
    VkAllocationCallbacks* allocationCallbacksPtr = nullptr;

    Allocator* create();

    Allocator* throwError(const VkResult result, const char* str) const;

private:
    VkAllocationCallbacks allocationCallbacks;

};


#endif //SPARKPLUG_ALLOCATOR_H
