#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 vVertex;
layout (location = 1) in vec3 vNormal; 
layout (location = 2) in vec2 texCoord;

layout (std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec4 normalColour;
    float normalLength;
} cameraUBO;

layout (push_constant) uniform MeshPushConstants {
	mat4 model;
	mat4 normal;
} meshPushConst;


layout (location = 0) out VertexStage {
    vec3 normal;
} vs_out;

void main() {
    gl_Position = cameraUBO.view * meshPushConst.model * vVertex; //camera space
    vs_out.normal = mat3(meshPushConst.normal) * vNormal; //normal
}