#version 440 core
layout(location=0) in vec4 vertex;
out vec2 coords;
uniform mat4 u_mproj;
void main()
{
	gl_Position = u_mproj * vec4(vertex.xy, 0.0, 1.0);
    coords = vertex.zw;
}