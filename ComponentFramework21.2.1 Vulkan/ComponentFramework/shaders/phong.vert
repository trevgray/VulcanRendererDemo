#version 450
#extension GL_ARB_separate_shader_objects : enable

//VERT SHADER RUNS FOR EVERY VERTEX

//Attributes--------------------- Update Every Frame
layout (location = 0) in vec4 vVertex;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 texCoord;

//Uniforms--------------------- Stay Uniform Unless Updated
layout(binding = 0) uniform CameraUBO { //uniform buffer
    mat4 view;
    mat4 proj;
} cameraUBO;

layout(binding = 2) uniform GlobalLightUBO {
	vec4 lightPos[4];
	vec4 lightColour[4];
} globalLightUBO;

layout (push_constant) uniform MeshPushConstants {
	mat4 model;
	mat4 normal;
} meshPushConst;

//OUTS---------------------
layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 lightDir[4];
layout (location = 5) out vec3 eyeDir;
layout (location = 6) out vec2 fragTextureCoords; //aka uv coords 
//uniform mat3 normalMatrix;

void main() {
	fragTextureCoords = texCoord;
	vertNormal = normalize(mat3(meshPushConst.normal) * vNormal.xyz); /// Rotate the normal to the correct orientation 
	vec3 vertPos = vec3(cameraUBO.view * meshPushConst.model * vVertex); /// This is the position of the vertex from the origin
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;
	for (int lightLoop = 0; lightLoop < 4; lightLoop++) {
 		lightDir[lightLoop] = normalize(vec3(globalLightUBO.lightPos[lightLoop]) - vertPos); /// Create the light direction. I do the math with in class 
		//vec3(ubo.lightPos) will downcast or ubo.lightPos.xyz will do the same
	}
//	if (meshPushConst.normal[1][2] < 0.0) {
//		gl_Position =  cameraUBO.proj * cameraUBO.view * vVertex;
//	}
	
	gl_Position =  cameraUBO.proj * cameraUBO.view * meshPushConst.model * vVertex; //you could do render_matrix * vVertex; - but we need the view to find the vertPos so why not
	
}
