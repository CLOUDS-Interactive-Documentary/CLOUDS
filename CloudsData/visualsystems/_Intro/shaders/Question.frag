#version 120

uniform vec4 tint;

void main (void)
{
	gl_FragColor = gl_Color * tint;
}
