#version 330 core
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos,1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
}