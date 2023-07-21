#version 450


layout (location = 0) in vec2 vertPosition;
layout (location = 1) in vec4 vertColor;

layout (push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
	vec3 color;
} push;


void main()
{
	gl_Position = vec4(
		push.transform * vertPosition + push.offset,
		// 0.0 is the front-most position
		0.0,
		// divide everything by 1.0 (nop)
		1.0);
}
