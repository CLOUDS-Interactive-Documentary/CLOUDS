#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData; 
uniform sampler2DRect accData;

uniform vec3 position; 
uniform float power; 
uniform float expFactor; 
uniform float limit; 

varying vec2 texcoord;

const float epsilon = 1e-6;

void main()
{    
	vec3 pos =  texture2DRect( posData,  texcoord ).xyz;	
 	vec3 acc =  texture2DRect( accData,  texcoord ).xyz;
	
	vec3 a = pos - position;		
	float m = length(a);
	if(m < epsilon || expFactor < epsilon){
		gl_FragColor = vec4(acc, 1.0);
	}
	else{
		a *= power * pow(exp(-m), expFactor);

		if(length(a) > limit)
		{
			//a = normalize(a)*limit;
			a = (a/m)*limit;
		}	

		gl_FragColor = vec4(acc+a, 1.0);
	}
}