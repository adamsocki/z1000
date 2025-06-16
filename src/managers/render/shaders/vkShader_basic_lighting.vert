#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 4) in vec4 instanceModelMatrix0;
layout(location = 5) in vec4 instanceModelMatrix1;
layout(location = 6) in vec4 instanceModelMatrix2;
layout(location = 7) in vec4 instanceModelMatrix3;

layout(location = 0) out vec3 fragPos;      // Fragment position in world space
layout(location = 1) out vec3 fragNormal;   // Normal in world space
layout(location = 2) out vec2 fragTexCoord;

void main() {
    mat4 instanceModelMatrix = mat4(
        instanceModelMatrix0,
        instanceModelMatrix1,
        instanceModelMatrix2,
        instanceModelMatrix3
    );


    // Calculate world position
    vec4 worldPos = instanceModelMatrix * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;

    fragNormal = mat3(instanceModelMatrix) * inNormal;

    fragTexCoord = inTexCoord;

    gl_Position = ubo.proj * ubo.view * worldPos;

//
//    gl_Position = ubo.proj * ubo.view * instanceModelMatrix * vec4(inPosition, 1.0);
//    fragColor = inColor;
//    fragTexCoord = inTexCoord;
}