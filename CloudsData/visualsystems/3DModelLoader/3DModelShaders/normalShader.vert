#version 120
#extension GL_ARB_texture_rectangle : enable

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main()
{
	norm = gl_NormalMatrix * gl_Normal;
	norm = norm;// * .5 + .5;
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
	uv = gl_MultiTexCoord0.xy;
}

