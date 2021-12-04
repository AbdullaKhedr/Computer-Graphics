//==================//
// Fragment Shader
//==================//
#version 330 core

struct Material
{
	vec3 ambient;
	sampler2D diffuseMap;
	vec3 specular;
	float shininess;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cosInnerCone;
	float cosOuterCone;
	int on;

	// Attenuation
	float constant;
	float linear;
	float exponent;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 fragColor;

uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

// Texture sampler
uniform sampler2D texSampler1;

vec3 calSpotLight(); // Prototype

void main()
{    
    // Ambient
	vec3 ambient = spotLight.ambient * vec3(texture(material.diffuseMap, TexCoord));
	
	vec3 spotColor = vec3(0.0f);
	if(spotLight.on == 1)
		spotColor = calSpotLight();
	
	fragColor = vec4(ambient + spotColor, 1.0f);
}

vec3 calSpotLight()
{
	vec3 lightDir = normalize(spotLight.position - FragPos);
	vec3 spotDir = normalize(spotLight.direction);

	float cosDir = dot(-lightDir, spotDir);
	float spotIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDir);
	
	// Diffuse
	vec3 normal = normalize(Normal);
	float NDotL = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = spotLight.diffuse * NDotL * vec3(texture(material.diffuseMap, TexCoord));

	// Specular -- blinn-Phong
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f); // Normal.HalfDir
	
	vec3 specular = spotLight.specular * material.specular * pow(NDotH, material.shininess);

	// Attenuation
	float d = length(spotLight.position - FragPos); // distance to the light
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * d 
	+ spotLight.exponent * (d * d));

	diffuse *= attenuation * spotIntensity;
	specular *= attenuation * spotIntensity;

	return(diffuse + specular);
}