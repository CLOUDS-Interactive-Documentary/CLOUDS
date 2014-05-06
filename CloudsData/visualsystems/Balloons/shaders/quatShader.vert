#version 110
#extension GL_ARB_texture_rectangle : enable

uniform float dimX;
uniform float dimY;

varying vec2 uv;

void main()
{
 	uv = gl_Vertex.xy * .5 + .5;
	uv *= vec2(dimX, dimY);
	
    gl_Position = gl_Vertex;
}