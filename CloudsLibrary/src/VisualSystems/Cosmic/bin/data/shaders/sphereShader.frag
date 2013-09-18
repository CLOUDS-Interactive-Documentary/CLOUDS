#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData; 
uniform sampler2DRect accData;

uniform vec3 position; 
uniform float radius; 
uniform float limit; 

varying vec2 texcoord;	

void main()
{    
	vec3 pos =  texture2DRect( posData,  texcoord ).xyz;
 	vec3 acc =  texture2DRect( accData,  texcoord ).xyz;
       	     
	vec3 positionOfSphere = position - normalize(position - pos)*radius; 
	vec3 a = positionOfSphere - pos; 

	float m = length(a); 
	if(m > limit)
	{
		a = normalize(a)*limit; 
	}	

  	gl_FragColor = vec4(acc+a, 1.0); 
}