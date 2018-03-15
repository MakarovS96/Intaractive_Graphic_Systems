#version 330 core
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 boneID;
layout (location = 4) in vec4 weight;

out vec3 lightDir;
out vec3 halfwayDir;
out vec3 Normal;
out vec3 viewDir;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 lightPos;
uniform vec4 viewPos;

const int size = 50;
uniform mat4 bones[size];

void main(void)
{
    mat4 boneTransform = bones[boneID[0]] * weight[0];
    boneTransform += bones[boneID[1]] * weight[1];
    boneTransform += bones[boneID[2]] * weight[2];
    boneTransform += bones[boneID[3]] * weight[3];

    vec4 bonePos = boneTransform * vec4(aPos, 1);
    vec3 FragPos = vec3(model * bonePos);

    lightDir = normalize(vec3(lightPos) - FragPos);
    viewDir = normalize(vec3(viewPos) - FragPos);
    halfwayDir = normalize(lightDir + viewDir);

    vec3 boneNormal = vec3(boneTransform * vec4(aNormal, 0.0));
    Normal = normalize(mat3(transpose(inverse(model))) * boneNormal);

    gl_Position = projection * view * projection * bonePos;
    TexCoord = aTexCoord;
}
