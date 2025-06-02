#version 450

// Match the existing uniform buffer layout exactly
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// Add optional texture sampler binding to match main shader
layout(binding = 1) uniform sampler2D texSampler;

// Push constants - must match the size in cpp code
layout(push_constant) uniform PushConstant {
    mat4 model_1;
    vec4 colliderColor; // Extra color data
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

// Match outputs with the main shader
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 colliderColor; // New output for our color

void main() {
    gl_Position = ubo.proj * ubo.view * pc.model_1 * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    colliderColor = pc.colliderColor;
}