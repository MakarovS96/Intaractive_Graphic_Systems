#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position; // Для направленного света не важна позиция источника
    vec3 direction; // Направление света для направленного света
    float cutOff; // Прожектор угол отсечения
    float outerCutOff; // Прожектор плавное затемнение по краям.

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    //vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = texture(material.diffuse, TexCoords).rgb * diff * light.diffuse;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.specular, TexCoords).rgb * spec * light.specular;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f/(light.constant + light.linear * distance + light.quadratic * distance * distance);

    //vec3 result = ambient + diffuse + specular;
    //vec3 result = (ambient + diffuse + specular) * attenuation;

    vec3 result;
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intencity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);

    if (theta > light.outerCutOff) {
        result = ambient + (diffuse + specular) * intencity;
    } else result = ambient;

    color = vec4(result, 1.0f);
}