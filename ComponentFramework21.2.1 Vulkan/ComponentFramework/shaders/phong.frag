#version 450
#extension GL_ARB_separate_shader_objects : enable

//FRAG SHADER RUNS FOR EVERY PIXEL OF THE SCREEN

layout (location = 0) in  vec3 vertNormal;
layout (location = 1) in  vec3 lightDir[4];
layout (location = 5) in  vec3 eyeDir;
layout (location = 6) in vec2 fragTextureCoords;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform GlobalLightUBO {
	vec4 lightPos[4];
	vec4 lightColour[4];
} globalLightUBO;

layout (location = 0) out vec4 fragColor;

void main() { 
	const vec4 ks = vec4(0.2, 0.2, 0.2, 0.0); //specular
	//vec4 kd[4] = {vec4(0.4, 0.1, 0.1, 0.0), vec4(0.1, 0.1, 0.4, 0.0), vec4(0.1, 0.4, 0.1, 0.0), vec4(0.1, 0.1, 0.1, 0.0)}; //colour of light - const means it cannot be changed just like C++
	vec4 ka[4] = {0.01 * globalLightUBO.lightColour[0], 0.01 * globalLightUBO.lightColour[1], 0.01 * globalLightUBO.lightColour[2], 0.01 * globalLightUBO.lightColour[3]}; //ambient light
	vec4 texColour = texture(texSampler, fragTextureCoords);
	//vec4 ka = texColour * 0.1;

	fragColor = vec4(0,0,0,0);
	
	//float diff[4]; //you could make arrays - but since it += i will just overwrite my variables
	//vec3 reflection[4];
	//float spec[4]
	for (int lightLoop = 0; lightLoop < 4; lightLoop++) {
		float diff = max(dot(vertNormal, lightDir[lightLoop]), 0.0);
		/// Reflection is based incident which means a vector from the light source
		/// not the direction to the light source
		vec3 reflection = normalize(reflect(-lightDir[lightLoop], vertNormal));
		float spec = max(dot(eyeDir, reflection), 0.0);
		if(diff > 0.0){
			spec = pow(spec,14.0);
		}
	fragColor += ka[lightLoop] + (diff * globalLightUBO.lightColour[lightLoop] * texColour) + (spec * ks); //phong shading
	}
} 