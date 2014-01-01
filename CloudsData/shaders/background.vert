#version 110
#extension GL_ARB_texture_rectangle : enable

varying vec2 oTexCoord;
void main()
{
	oTexCoord = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
}

