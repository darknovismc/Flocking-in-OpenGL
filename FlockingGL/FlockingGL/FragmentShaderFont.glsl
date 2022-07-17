#version 440 core
layout(location=0) out vec4 color;
in vec2 coords;
uniform sampler2D text;
uniform vec4 u_Color;
void main()
{
	color = vec4(u_Color.rgb, texture(text, coords).r);
}