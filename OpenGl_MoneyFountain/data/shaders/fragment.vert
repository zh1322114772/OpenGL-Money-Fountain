#version 330 core
in vec2 coord;
uniform sampler2D tex0;
out vec4 fragColor;

void main()
{
	fragColor = texture(tex0, coord);
}