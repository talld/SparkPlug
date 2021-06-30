
#define VMA_IMPLEMENTATION

#include <glm/vec4.hpp>
#include "Engine/Renderer/Renderer.h"


int main() {

    glm::vec4 vect;

    vect*=3;

    Renderer renderer;
    renderer.create();

    return 0;

}
