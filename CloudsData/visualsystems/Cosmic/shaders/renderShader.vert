#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData;
uniform sampler2DRect radiData;
uniform float radiusMultiplier;      

varying vec2 texcoord;

void main()
{
	texcoord = gl_MultiTexCoord0.st;
	vec4 pos = texture2DRect( posData, texcoord.st);	
	vec4 radi = texture2DRect( radiData, texcoord.st); 
	
	gl_PointSize = radi.x*radiusMultiplier; 
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;	
	gl_FrontColor = gl_Color;     	
}
