#version 330
#extension GL_NV_shadow_samplers_cube : enable

in vec3 viewDir;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform samplerCube cubeMap;

/*void main(void)
{
    vec3 r = reflect(-normalize(viewDir), normalize(Normal));
    vec4 diffColor = texture(cubeMap, r);
    FragColor = 0.8 * (diffColor);
}*/

void main()
{
    float ratio = 1.00 / 1.52;
    vec3 RR = refract(-normalize(viewDir), normalize(Normal), ratio);

    FragColor = 0.8 * vec4(texture(cubeMap, RR).rgb, 1.0);
}