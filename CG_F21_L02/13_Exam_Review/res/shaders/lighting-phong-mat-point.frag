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

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Attenuation
	float constant;
	float linear;
	float exponent;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 fragColor;

uniform PointLight pointLight;
uniform Material material;
uniform vec3 viewPos;

// Texture sampler
uniform sampler2D texSampler1;

void main()
{    
    // Ambient
	vec3 ambient = pointLight.ambient * material.ambient;

	// Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(pointLight.position - FragPos);
	float NDotL = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = pointLight.diffuse * NDotL * vec3(texture(material.diffuseMap, TexCoord));

	// Specular -- blinn-Phong
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f); // Normal.HalfDir
	
	vec3 specular = pointLight.specular * material.specular * pow(NDotH, material.shininess);

	// Attenuation
	float d = length(pointLight.position - FragPos); // distance to the light
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear * d 
	+ pointLight.exponent * (d * d));

	diffuse *= attenuation;
	specular *= attenuation;

	fragColor = vec4(ambient + diffuse + specular, 1.0f);
}