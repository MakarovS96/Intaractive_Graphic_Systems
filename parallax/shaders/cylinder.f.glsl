

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 halfpathTangent;
in vec3 lightTangent;
in vec3 viewTangent;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight) {
    float minLayers = 300 * abs(heightScale);
    float maxLayers = 500 * abs(heightScale);
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));
    float layerHeight = 1.0 / numLayers;
    float curLayerHeight = 0;
    vec2 dtex = heightScale * V.xy / (V.z * numLayers);
    vec2 currentTextureCoords = T;
    float heightFromTexture = texture(depthMap, currentTextureCoords).r;

    while(heightFromTexture > curLayerHeight)
    {
        curLayerHeight += layerHeight;
        currentTextureCoords -= dtex;
        heightFromTexture = texture(depthMap, currentTextureCoords).r;
    }

    vec2 prevTCoords = currentTextureCoords + dtex;
    float nextH = heightFromTexture - curLayerHeight;
    float prevH = texture(depthMap, prevTCoords).r - curLayerHeight + layerHeight;
    float weight = nextH / (nextH - prevH);
    vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);
    parallaxHeight = curLayerHeight + prevH * weight + nextH * (1.0 - weight);

    return finalTexCoords;
}

void main()
{
    float parallaxHeight;
    vec2 texCoord = TexCoords;

    texCoord = parallaxMapping(viewTangent, TexCoords, parallaxHeight);

    vec3 diffColor = texture2D(diffuseMap, texCoord).rgb;
    vec3 ambColor = vec3(0.3f, 0.3f, 0.3f) * diffColor;
    const vec3	specColor = vec3(0.5f, 0.5f, 0.5f);
    const float specPower = 30.0f;
    vec3 normal = normalize(texture2D(normalMap, texCoord).rgb * 2.0f - vec3(1.0f));
    vec3 lightDirT = normalize(lightTangent);
    vec3 halfpathDirT = normalize(halfpathTangent);
    vec3 diff = diffColor * max(dot(normal, lightDirT), 0.0f);
    vec3 spec = specColor * pow(max(dot(normal, halfpathDirT), 0.0f), specPower);
    FragColor = vec4(ambColor + diff + spec, 1.0f);
}