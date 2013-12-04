#version 110
#extension GL_ARB_texture_rectangle : enable

uniform float trailLength;
varying vec2 texcoord;

void main()  
{  
 	texcoord = vec2(gl_MultiTexCoord0.s, gl_MultiTexCoord0.t * trailLength);
	//texcoord = vec2(gl_Vertex.x, gl_Vertex.y * trailLength);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
} 