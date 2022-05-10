#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

out vec4 fColor;

uniform samplerCube skybox;
uniform sampler2D diffuseTexture;

uniform float fogDensity;

float computeFog() {
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
    return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
    vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;


    vec4 fogColor = vec4 (0.5f,0.5f,0.5f,1.0f);
    float fogFactor = computeFog();

    vec3 cameraPosEye = vec3(0.0f);
    vec3 viewDirection = cameraPosEye - fPosEye.xyz;
    vec3 viewDirectionN = normalize(viewDirection);
    vec3 normalN = normalize(fNormal);
    vec3 reflection = reflect(viewDirectionN, normalN);
    vec3 colorFromSkybox = vec3(texture(skybox, reflection));

    vec4 color = vec4(colorFromTexture.rgb, 0.5f);
    fColor = mix(fogColor, color, fogFactor);
    //fColor = mix(fogColor, vec4(colorFromSkybox, 0.3f), fogFactor);

}