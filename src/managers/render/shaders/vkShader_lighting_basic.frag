#version 450

// Basic lighting fragment shader following LearnOpenGL Colors tutorial
// This implements: result = lightColor * objectColor

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightingUniformBuffer {
    vec3 lightColor;
    vec3 objectColor;
} lighting;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // Step 1 of LearnOpenGL lighting tutorial
    // Simple multiplication: result = lightColor * objectColor
    vec3 result = lighting.lightColor * lighting.objectColor;
    outColor = vec4(result, 1.0);
}