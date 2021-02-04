#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normVec;

uniform mat4 model;
uniform vec3 modelPos;
uniform mat4 view;
uniform mat4 projection;

out vec2 coord;
out vec3 normal;
out vec3 fragPos;

void main()
{
	vec4 worldPosition = model * vec4(pos.xyz, 1.0);

	gl_Position =  projection * view * worldPosition;

	coord = texCoord;
	fragPos = worldPosition.xyz;
	normal = normalize((model * vec4(normVec, 1.0)).xyz - modelPos);

}