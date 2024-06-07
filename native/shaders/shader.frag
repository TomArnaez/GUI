#version 450

layout(binding = 1) uniform usampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    float color = float(texture(texSampler, fragTexCoord).r) / 255.0;
    outColor = vec4(color, color, color, 1.0);
}