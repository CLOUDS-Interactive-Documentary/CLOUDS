
#version 120
#extension GL_ARB_texture_rectangle : enable


varying vec3 ePos;
varying float dist;
varying vec2 endVal;

void main()
{
	endVal = gl_Vertex.xz;// / halfGridDim;
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
	dist = gl_Position.z;
	
	gl_FrontColor = gl_Color;
}

