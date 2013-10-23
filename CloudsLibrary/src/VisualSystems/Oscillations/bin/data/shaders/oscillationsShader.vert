#version 110

void main(){

	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 pos = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	gl_Position = pos;
	gl_FrontColor =  gl_Color;

}
