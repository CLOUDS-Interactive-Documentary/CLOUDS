#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect acceleration;
uniform sampler2DRect force;

uniform float time; 
uniform float timestep; 
uniform float accLimit; 
uniform float damping; 

varying vec2 texcoord;	

void main()
{
 	vec3 accel =  texture2DRect( acceleration, texcoord ).xyz;
 	vec3 force =  texture2DRect( force, texcoord ).xyz;
 
 	accel += force;

	float m = length(acc); 
	if(m > accLimit)
	{
		accel = normalize(acc)*accLimit; 
	}	

  	gl_FragColor = vec4(accel, 1.0); 
}