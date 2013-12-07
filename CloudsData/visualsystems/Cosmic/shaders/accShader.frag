#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect accData;
uniform sampler2DRect velData;

uniform float time; 
uniform float timestep; 
uniform float accLimit; 
uniform float damping; 

varying vec2 texcoord;	

void main()
{    
 	vec3 acc =  texture2DRect( accData,  texcoord ).xyz;
 	vec3 vel =  texture2DRect( velData,  texcoord ).xyz;
 
 	acc -= vel * damping; 

	float m = length(acc); 
	if(m > accLimit)
	{
		acc = normalize(acc)*accLimit; 
	}	

  	gl_FragColor = vec4(acc, 1.0); 
}