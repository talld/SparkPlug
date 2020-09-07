#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragmentColor;

void main() {
    gl_Position = vec4(position,1.0);

    fragmentColor = color;
}	