#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;

out vec4 fragPosLightSpace;
uniform mat4 lightSpaceTrMatrix;

uniform vec3 uPosLanternLight; //revs
uniform vec3 uPosStarLight; //revs

out vec3 posLanternLight;
out vec3 posStarLight;


void main() 
{
	posLanternLight = (view * model * vec4(uPosLanternLight, 1.0f)).xyz;
	posStarLight = (view * model * vec4(uPosStarLight, 1.0f)).xyz;

	fragPosLightSpace = lightSpaceTrMatrix*model*vec4(vPosition,1.0f);
	//compute eye space coordinates
	fPosEye = view * model * vec4(vPosition, 1.0f);
	fNormal = normalize(normalMatrix * vNormal);
	fTexCoords = vTexCoords;
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
