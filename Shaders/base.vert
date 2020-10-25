#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(binding = 0) uniform MVP{ //these could just be one premultiplied matrix however the gpu isnt exactly doing much at this stage
	mat4 projection;
	mat4 view; 
	mat4 model;
} mvp;

layout(location = 0) out vec3 fragmentColor;

void main() {
    gl_Position = mvp.projection * mvp.view * mvp.model * vec4(position,1.0);

    fragmentColor = color;
}	