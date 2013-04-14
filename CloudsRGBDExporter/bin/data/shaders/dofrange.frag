#version 110
#extension GL_ARB_texture_rectangle : enable

varying float VInFocus0;
varying float VZPositionValid0;
uniform float blackout;

void main()
{
	if(VZPositionValid0 < 0.999){ 
    	discard;
    }	
	gl_FragColor = vec4(VInFocus0) * blackout;
}