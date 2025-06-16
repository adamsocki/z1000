#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform PhongMaterialUBO {
    vec3 ambient;
    float pad1;
    vec3 diffuse;
    float pad2;
    vec3 specular;
    float shininess;
    
    vec3 lightPosition;
    float pad3;
    vec3 lightAmbient;
    float pad4;
    vec3 lightDiffuse;
    float pad5;
    vec3 lightSpecular;
    float pad6;
    
    vec3 viewPos;
    float pad7;
} material;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 norm = normalize(fragNormal);
    
    // Ambient
    vec3 ambient = material.lightAmbient * material.ambient;
    
    // Diffuse
    vec3 lightDir = normalize(material.lightPosition - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.lightDiffuse * (diff * material.diffuse);
    
    // Specular (Phong)
    vec3 viewDir = normalize(material.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.lightSpecular * (spec * material.specular);
    
    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, 1.0);
}