#version 330 core
in vec2 coord;
in vec3 normal;
in vec3 fragPos;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform sampler2D tex0;


out vec4 fragColor;

void main()
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 result;

	//ambient light
	float ambientStrength = 0.25;
	ambient = lightColor * ambientStrength;

	//diffuse light
	vec3 lightDirection = normalize(lightPosition - fragPos);
	float diffuseStrength = 0.75;
	diffuse = (max(dot(normal, lightDirection), 0.0f) * diffuseStrength) * lightColor;

	//specular light
	float shininess = 64;
	float specularStrength = 1;

	vec3 lightReflectDirection = reflect(-lightDirection, normal);
	vec3 cameraDirection = normalize(cameraPosition - fragPos);
	specular = pow(max(dot(cameraDirection, lightReflectDirection), 0.0) * specularStrength, shininess) * lightColor ;
	
	//blend color
	result = (ambient + diffuse + specular);
	fragColor = texture(tex0, coord) * vec4(result, 1.0);
}