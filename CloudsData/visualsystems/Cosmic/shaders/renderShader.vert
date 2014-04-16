#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData;
uniform sampler2DRect radiData;
uniform float radiusMultiplier;      
uniform float size; 
varying vec2 texcoord;
varying vec4 color;

void main()
{
	texcoord = gl_MultiTexCoord0.st;
    color = gl_Color; 
	vec2 lookUp = gl_Vertex.xy; 
	vec4 offsetPos = vec4((texcoord/size) - 0.5, 0.0, 0.0); 
	vec4 pos = texture2DRect( posData, lookUp);	
	float radi = texture2DRect( radiData, lookUp).x; 	

	offsetPos*=(radi)*radiusMultiplier; 
	
	pos = gl_ModelViewMatrix * pos; 
	pos += offsetPos; 
	gl_Position = gl_ProjectionMatrix * pos; 
}
