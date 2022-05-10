#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

out vec4 fColor;

in vec4 fragPosLightSpace;


//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//pentru lumina punctiforma
uniform float constant;
uniform float linear;
uniform float quadratic;
in vec3 posLanternLight;
in vec3 posStarLight;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform float fogDensity;
uniform float lightIntensity;


vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

vec3 colorLanternLight=vec3(1.0f, 0.411f, 0.705f);
//vec3 posLanternLight=vec3(2.9531f,2.9192f,0.64537f);
vec3 colorStarLight=vec3(0.5f, 1.0f, 0.8f);

float computeShadow() {
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    if (normalizedCoords.z > 1.0f)
    return 0.0f;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}

vec3 computePointLight(vec3 lightPosEye, vec3 lightColor, vec4 colorFromTexture) {
    vec3 cameraPosEye = vec3(0.0f);
    vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);
    vec3 normalEye = normalize(fNormal);
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    float diffCoeff = max(dot(normalEye, lightDirN), 0.0f);
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflectDir), 0.0f), shininess);

    float distance = length(lightPosEye - fPosEye.xyz);
    float attenuation = 1.0f / (constant + linear * distance + quadratic * distance * distance);

    vec3 ambient = ambientStrength * lightColor * colorFromTexture.rgb;
    vec3 diffuse = diffCoeff * lightColor * colorFromTexture.rgb;
    vec3 specular = specularStrength * specCoeff * lightColor * colorFromTexture.rgb;

    ambient     *= attenuation;
    diffuse     *= attenuation;
    specular    *= attenuation;

    vec3 color = min(ambient + diffuse + specular, 1.0f);
    return color;
}

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeFog()
{
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}


void main() 
{

	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;

	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);
	//vec3 color = min((ambient + diffuse) + specular, 1.0f);
	// fColor = vec4(color, 1.0f);

	color += computePointLight(posLanternLight,colorLanternLight,colorFromTexture);
	color += computePointLight(posStarLight,colorStarLight,colorFromTexture);

	//ceata
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	fColor = mix(fogColor, vec4(color, colorFromTexture.a), fogFactor);
   
}


