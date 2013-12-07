#version 110
#extension GL_ARB_texture_rectangle : enable


//JG Fixed rectangular sample bug
//uniform sampler2DRect glow;
uniform sampler2D glow;
uniform float spriteSize;

varying vec2 texcoord;

void main()
{
//	vec4 color = texture2DRect( glow, texcoord.st*spriteSize);
	vec4 color = texture2D( glow, texcoord.st );
    gl_FragColor =  color * gl_Color;        
}