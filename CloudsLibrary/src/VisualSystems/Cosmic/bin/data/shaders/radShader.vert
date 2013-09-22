#version 110
#extension GL_ARB_texture_rectangle : enable

varying vec2 texcoord;
  
void main()  
{  
 	texcoord = gl_MultiTexCoord0.st; 	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
} 