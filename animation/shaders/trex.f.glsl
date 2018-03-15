#version 330 core

in vec3 lightDir;
in vec3 halfwayDir;
in vec3 viewDir;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textureSampler;

void main(void)
{
    vec4 diffColor = texture2D(textureSampler, TexCoord);
    vec4 ambColor = vec4(0.3, 0.3, 0.3, 1.0) * diffColor;

    const vec4	specColor = vec4(0.5, 0.5, 0.5, 1.0);
    const float specPower = 30.0;

    vec3 n2 = normalize(Normal);
    vec3 l2 = normalize(lightDir);
    vec3 h2 = normalize(halfwayDir);

    vec4 diff = diffColor * max(dot(n2, l2), 0.0);
    vec4 spec = specColor * pow(max(dot(n2, h2), 0.0), specPower);

    FragColor = ambColor + diff + spec;
}