#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightingUniformBuffer {
    vec3 lightColor;
    vec3 objectColor;
    vec3 lightPos;
    vec3 viewPos;
    
    // Material properties (LearnOpenGL Materials tutorial)
    vec3 materialAmbient;
    vec3 materialDiffuse;
    vec3 materialSpecular;
    float materialShininess;
    
    // Legacy properties
    float ambientStrength;
    float specularStrength;
    int shininess;
    int lightingMode;
    int useMaterialProperties;
} lighting;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 result;

    // Normalize the fragment normal
    vec3 norm = normalize(fragNormal);

    // Choose between material properties and legacy lighting
    if (lighting.useMaterialProperties == 1) {
        // Use material properties (LearnOpenGL Materials tutorial)
        switch(lighting.lightingMode) {
            case 0: // LIGHTING_MODE_SIMPLE_COLOR
            result = lighting.lightColor * lighting.materialDiffuse;
            break;

            case 1: // LIGHTING_MODE_AMBIENT_ONLY
            vec3 ambient = lighting.lightColor * lighting.materialAmbient;
            result = ambient;
            break;

            case 2: // LIGHTING_MODE_DIFFUSE
            vec3 ambient_1 = lighting.lightColor * lighting.materialAmbient;
            vec3 lightDir_1 = normalize(lighting.lightPos - fragPos);
            float diff_1 = max(dot(norm, lightDir_1), 0.0);
            vec3 diffuse_1 = diff_1 * lighting.lightColor * lighting.materialDiffuse;
            result = ambient_1 + diffuse_1;
            break;

            case 3: // LIGHTING_MODE_SPECULAR (Full material properties)
            vec3 ambient_2 = lighting.lightColor * lighting.materialAmbient;
            vec3 lightDir = normalize(lighting.lightPos - fragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lighting.lightColor * lighting.materialDiffuse;
            vec3 viewDir = normalize(lighting.viewPos - fragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), lighting.materialShininess);
            vec3 specular = spec * lighting.lightColor * lighting.materialSpecular;
            result = ambient_2 + diffuse + specular;
            break;

            default:
            result = lighting.materialDiffuse;
            break;
        }
    } else {
        // Use legacy lighting (backward compatibility)
        switch(lighting.lightingMode) {
            case 0: // LIGHTING_MODE_SIMPLE_COLOR
            result = lighting.lightColor * lighting.objectColor;
            break;

            case 1: // LIGHTING_MODE_AMBIENT_ONLY
            vec3 ambient = lighting.ambientStrength * lighting.lightColor;
            result = ambient * lighting.objectColor;
            break;

            case 2: // LIGHTING_MODE_DIFFUSE
            vec3 ambient_1 = lighting.ambientStrength * lighting.lightColor;
            vec3 lightDir_1 = normalize(lighting.lightPos - fragPos);
            float diff_1 = max(dot(norm, lightDir_1), 0.0);
            vec3 diffuse_1 = diff_1 * lighting.lightColor;
            result = (ambient_1 + diffuse_1) * lighting.objectColor;
            break;

            case 3: // LIGHTING_MODE_SPECULAR
            vec3 ambient_2 = lighting.ambientStrength * lighting.lightColor;
            vec3 lightDir = normalize(lighting.lightPos - fragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lighting.lightColor;
            vec3 viewDir = normalize(lighting.viewPos - fragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), lighting.shininess);
            vec3 specular = lighting.specularStrength * spec * lighting.lightColor;
            result = (ambient_2 + diffuse + specular) * lighting.objectColor;
            break;

            default:
            result = lighting.objectColor;
            break;
        }
    }

    outColor = vec4(result, 1.0);
}