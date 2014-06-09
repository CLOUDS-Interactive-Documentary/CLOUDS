#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 color;
void main (void)
{
	gl_FragColor = color * gl_Color.a;
	
}
