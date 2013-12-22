
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float pointSize;

void main(void){
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
	//gl_PointSize = 1.0 + (gl_Normal.y * pointSize);
	gl_PointSize = gl_Normal.y*pointSize;
}
