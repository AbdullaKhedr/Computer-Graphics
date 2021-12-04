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

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 fragColor;

uniform DirectionalLight dirLight;
uniform Material material;
uniform vec3 viewPos;

// Texture sampler
uniform sampler2D texSampler1;

void main()
{    
    // Ambient
	vec3 ambient = dirLight.ambient * material.ambient;

	// Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction); // Only direction (No Position)
	float NDotL = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = dirLight.diffuse * NDotL * vec3(texture(material.diffuseMap, TexCoord));

	// Specular -- blinn-Phong
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f); // Normal.HalfDir
	vec3 specular = dirLight.specular * material.specular * pow(NDotH, material.shininess);

	vec4 texel = texture(texSampler1, TexCoord);
	fragColor = vec4(ambient + diffuse + specular, 1.0f) * texel;
}