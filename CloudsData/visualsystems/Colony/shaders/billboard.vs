#version 120

varying float size;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
    
    size = gl_Normal.x * 18.;
	gl_PointSize = size;

	gl_FrontColor = gl_Color;
}





