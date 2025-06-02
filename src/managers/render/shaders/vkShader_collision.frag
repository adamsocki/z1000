#version 450

// Match the existing layout from main shader
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 colliderColor;

layout(location = 0) out vec4 outColor;

void main() {
    // Use the color from the push constant for collision visualization
    // Ignore texture - wireframe mode doesn't need texturing
    outColor = colliderColor;
}