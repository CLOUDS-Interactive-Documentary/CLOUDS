#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect glow;
uniform float spriteSize

varying vec2 texcoord;

void main()
{
	vec4 color = texture2DRect( glow, texcoord.st*spriteSize);
    gl_FragColor =  color * gl_Color;        
}