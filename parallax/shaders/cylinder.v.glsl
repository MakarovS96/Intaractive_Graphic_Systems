#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 halfpathTangent;
out vec3 lightTangent;
out vec3 viewTangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0f));
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfpath = normalize(lightDir + viewDir);
    vec3 normal = normalize(aNormal);

    vec3 tn = normalize(aTangent);
    vec3 bn = normalize(aBitangent);

    lightTangent = vec3(dot(lightDir, tn), dot(lightDir, bn), dot(lightDir, normal));
    halfpathTangent = vec3(dot(halfpath, tn), dot(halfpath, bn), dot(halfpath, normal));
    viewTangent = vec3(dot(viewDir, tn), dot(viewDir, bn), dot(viewDir, normal));

    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    TexCoords = aTexCoords;
}