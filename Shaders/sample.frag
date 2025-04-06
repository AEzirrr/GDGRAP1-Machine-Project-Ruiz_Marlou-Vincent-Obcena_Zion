#version 330 core

uniform sampler2D tex0;
uniform sampler2D norm_tex;

// Directional light properties
uniform vec3 dirLightDirection;    // Direction of the directional light
uniform vec3 dirLightColor;        // Color of the directional light
uniform float dirLightBrightness;  // Brightness of the directional light

// Point light properties
uniform vec3 pointLightPos;        // Position of the point light
uniform vec3 pointLightColor;      // Color of the point light
uniform float pointLightBrightness; // Brightness of the point light

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float brightness; // Add brightness parameter

uniform vec3 ambientColor;
uniform float ambientStr;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

out vec4 FragColor;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

in mat3 TBN;

// Function to calculate diffuse and specular lighting for a given light
vec3 calculateLight(vec3 lightDir, vec3 lightColor, float brightness, float attenuation, vec3 normal, vec3 viewDir) {
    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * attenuation * brightness;

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    vec3 specular = spec * specStr * lightColor * attenuation * brightness;

    return diffuse + specular;
}

void main()
{
    vec4 pixelColor = texture(tex0, texCoord);

    if(pixelColor.a < 0.1){
        discard;
    }

    // Use normal from normal map
    vec3 normal = texture(norm_tex, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 viewDir = normalize(cameraPos - fragPos);

    // Directional light calculation
    vec3 dirLightDir = normalize(-dirLightDirection); // Directional light direction is constant
    vec3 dirLight = calculateLight(dirLightDir, dirLightColor, dirLightBrightness, 1.0, normal, viewDir);

    // Point light calculation
    float pointLightDist = length(pointLightPos - fragPos); // Calculate the distance between the fragment and point light
    float pointLightAttenuation = 1.0 / (1.0 + 0.05 * pointLightDist + 0.01 * pointLightDist * pointLightDist); // Attenuation formula
    vec3 pointLightDir = normalize(pointLightPos - fragPos);
    vec3 pointLight = calculateLight(pointLightDir, pointLightColor, pointLightBrightness, pointLightAttenuation, normal, viewDir);

    // Main light 
    float lightDist = length(lightPos - fragPos); // Calculate the distance to the main light
    float lightAttenuation = 1.0 / (1.0 + 0.05 * lightDist + 0.01 * lightDist * lightDist); // Attenuation for light
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightAttenuation * brightness;

    vec3 ambientCol = ambientColor * ambientStr; // Ambient lighting

    // Specular lighting calculation
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    vec3 specColor = spec * specStr * lightColor * lightAttenuation * brightness;

    // Final fragment color
    FragColor = vec4((ambientCol + diffuse + specColor + pointLight + dirLight), 1.0) * texture(tex0, texCoord);
}
