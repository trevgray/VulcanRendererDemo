#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 vVertex;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 texCoord;

layout(binding = 0) uniform UniformBufferObject { //uniform buffer
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 lightPos;
} ubo;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 lightDir;
layout (location = 2) out vec3 eyeDir;
layout (location = 3) out vec2 fragTextureCoords; //aka uv coords 
//uniform mat3 normalMatrix;

void main() {
	fragTextureCoords = texCoord;
	mat3 normalMatrix = mat3(transpose(inverse(ubo.model)));
	vertNormal = normalize(normalMatrix * vNormal.xyz); /// Rotate the normal to the correct orientation 
	vec3 vertPos = vec3(ubo.view * ubo.model * vVertex); /// This is the position of the vertex from the origin
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;
	lightDir = normalize(vec3(ubo.lightPos) - vertPos); /// Create the light direction. I do the math with in class 
	//vec3(ubo.lightPos) will downcast or ubo.lightPos.xyz will do the same
	gl_Position =  ubo.proj * ubo.view * ubo.model * vVertex; 
}
