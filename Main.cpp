#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION
#include <iostream>
#include <Engine.h>

int main() {
    Engine engine;
    engine.boot();
    return 0;
}
