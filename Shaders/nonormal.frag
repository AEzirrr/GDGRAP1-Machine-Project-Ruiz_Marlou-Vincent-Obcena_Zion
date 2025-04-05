#version 330 core

#define MAX_POINT_LIGHTS 10

uniform sampler2D tex0;

// Directional light properties
uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;
uniform float dirLightBrightness;

// Point lights
uniform int numPointLights;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];
uniform float pointLightBrightness[MAX_POINT_LIGHTS];

// Ambient light
uniform vec3 ambientColor;
uniform float ambientStr;

// Camera and material properties
uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

out vec4 FragColor;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

// Function to calculate diffuse and specular lighting
vec3 calculateLight(vec3 lightDir, vec3 lightColor, float brightness, float attenuation, vec3 normal, vec3 viewDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * attenuation * brightness;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    vec3 specular = spec * specStr * lightColor * attenuation * brightness;

    return diffuse + specular;
}

void main()
{
    vec4 pixelColor = texture(tex0, texCoord);
    if (pixelColor.a < 0.1) discard;

    vec3 normal = normalize(normCoord);
    vec3 viewDir = normalize(cameraPos - fragPos);

    // Ambient light
    vec3 ambientCol = ambientColor * ambientStr;

    // Directional light
    vec3 dirLightDir = normalize(-dirLightDirection);
    vec3 dirLight = calculateLight(dirLightDir, dirLightColor, dirLightBrightness, 1.0, normal, viewDir);

    // Point lights loop
    vec3 totalPointLight = vec3(0.0);
    for (int i = 0; i < numPointLights; ++i) {
        vec3 pointLightDir = normalize(pointLightPos[i] - fragPos);
        float distance = length(pointLightPos[i] - fragPos);
        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.01 * distance * distance);
        totalPointLight += calculateLight(pointLightDir, pointLightColor[i], pointLightBrightness[i], attenuation, normal, viewDir);
    }

    // Final output
    vec3 totalLight = ambientCol + dirLight + totalPointLight;
    FragColor = vec4(totalLight, 1.0) * pixelColor;
}
