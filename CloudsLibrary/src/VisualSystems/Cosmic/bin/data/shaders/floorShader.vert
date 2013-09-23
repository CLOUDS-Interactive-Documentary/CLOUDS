#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData;
uniform sampler2DRect radiData;
uniform float radiusMultiplier;      
uniform float size; 
uniform float shadowScale; 

varying float posZ; 
varying vec2 texcoord;

void main()
{
	texcoord = gl_MultiTexCoord0.st;	
	vec2 lookUp = gl_Vertex.xy; 
	vec3 shadowPos = vec3(texcoord/size, 0.0) + vec3(-0.5, -0.5, 0.0); 	
	vec3 pos = texture2DRect( posData, lookUp).xyz;	
	float radi = texture2DRect( radiData, lookUp).x; 	
	
	shadowPos*=(radi)*shadowScale; 
	posZ = pos.z; 

	pos.z = -size; 
	shadowPos+=pos; 	
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(shadowPos, 1.0);			   	
}
