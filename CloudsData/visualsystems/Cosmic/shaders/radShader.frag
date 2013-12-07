#version 110
#extension GL_ARB_texture_rectangle : enable

#define PI 3.14159265359

uniform float time; 
uniform float size; 
varying vec2 texcoord;	

void main()
{   
	float value = .25+.75*abs(cos((texcoord.x/size)*PI+time)*sin((texcoord.y/size)*PI+time)); 

  	gl_FragColor = vec4(value); 
}

