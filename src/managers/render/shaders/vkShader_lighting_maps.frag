#version 450

layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D specularMap;

layout(binding = 3) uniform LightingMapsUBO {
    float shininess;
    float pad1;
    float pad2;
    float pad3;
    
    vec3 lightPosition;
    float pad4;
    vec3 lightAmbient;
    float pad5;
    vec3 lightDiffuse;
    float pad6;
    vec3 lightSpecular;
    float pad7;
    
    vec3 viewPos;
    float pad8;
} material;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 norm = normalize(fragNormal);
    
    // Sample diffuse and specular maps
    vec3 diffuseColor = vec3(texture(diffuseMap, fragTexCoord));
    vec3 specularColor = vec3(texture(specularMap, fragTexCoord));
    
    // Ambient
    vec3 ambient = material.lightAmbient * diffuseColor;
    
    // Diffuse
    vec3 lightDir = normalize(material.lightPosition - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.lightDiffuse * (diff * diffuseColor);
    
    // Specular
    vec3 viewDir = normalize(material.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.lightSpecular * (spec * specularColor);
    
    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, 1.0);
}