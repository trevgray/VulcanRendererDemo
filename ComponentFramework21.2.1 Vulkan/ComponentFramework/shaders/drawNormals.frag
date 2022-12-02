#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec4 normalColour;
    float normalLength;
} cameraUBO;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = cameraUBO.normalColour;
}