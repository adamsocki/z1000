#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightingUniformBuffer {
    vec3 lightColor;
    vec3 objectColor;
    vec3 lightPos;
    vec3 viewPos;
    float ambientStrength;
    float specularStrength;
    int shininess;
    int lightingMode;
} lighting;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 result;

    // Normalize the fragment normal
    vec3 norm = normalize(fragNormal);

    switch(lighting.lightingMode) {
        case 0: // LIGHTING_MODE_SIMPLE_COLOR - Red tint for debug
        result = lighting.lightColor * lighting.objectColor;
        result.r += 0.3; // Add red tint to verify this mode
        break;

        case 1: // LIGHTING_MODE_AMBIENT_ONLY - Green tint for debug
        vec3 ambient = lighting.ambientStrength * lighting.lightColor;
        result = ambient * lighting.objectColor;
        result.g += 0.3; // Add green tint to verify this mode
        break;

        case 2: // LIGHTING_MODE_DIFFUSE - Blue tint for debug
        vec3 ambient_1 = lighting.ambientStrength * lighting.lightColor;
        vec3 lightDir_1 = normalize(lighting.lightPos - fragPos);
        float diff_1 = max(dot(norm, lightDir_1), 0.0);
        vec3 diffuse_1 = diff_1 * lighting.lightColor;
        result = (ambient_1 + diffuse_1) * lighting.objectColor;
        result.b += 0.3; // Add blue tint to verify this mode
        break;

        case 3: // LIGHTING_MODE_SPECULAR - Purple tint for debug
        vec3 ambient_2 = lighting.ambientStrength * lighting.lightColor;
        vec3 lightDir = normalize(lighting.lightPos - fragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lighting.lightColor;
        vec3 viewDir = normalize(lighting.viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), lighting.shininess);
        vec3 specular = lighting.specularStrength * spec * lighting.lightColor;
        result = (ambient_2 + diffuse + specular) * lighting.objectColor;
        result.r += 0.2; result.b += 0.2; // Add purple tint to verify this mode
        break;

        default:
        result = vec3(1.0, 0.0, 1.0); // Bright magenta for debug - should never show
        break;
    }

    outColor = vec4(result, 1.0);
}