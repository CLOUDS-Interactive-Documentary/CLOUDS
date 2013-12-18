#version 110
#extension GL_ARB_texture_rectangle : enable

//gl_Normal.x == pop decay, if x == 1.0, fully bright, if x == 0., default
//gl_Normal.y == tells us if we are in the middle of the line or on the node

varying float popMix;
varying float baseColorMix;
void main(){
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
	
	
}

