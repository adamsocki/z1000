#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform UnlitMaterialUBO {
    vec4 color;
} material;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = material.color;
}