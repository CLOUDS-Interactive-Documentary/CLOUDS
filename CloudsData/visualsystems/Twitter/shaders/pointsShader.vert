
#version 120
#extension GL_ARB_texture_rectangle : enable

//uniform float pointSize;
uniform float minSize;
uniform float maxSize;
uniform vec3 attractor;

void main(void){
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//	gl_FrontColor = gl_Color;
//	//gl_PointSize = 1.0 + (gl_Normal.y * pointSize);
//	gl_PointSize = gl_Normal.y*pointSize;

	gl_PointSize = 7.0;
	gl_FrontColor = gl_Color;//*nearAttenuate;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
