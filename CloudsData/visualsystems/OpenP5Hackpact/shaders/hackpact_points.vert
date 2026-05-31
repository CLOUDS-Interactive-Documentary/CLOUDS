#version 120

uniform float maxPointSize;

void main(void) {
	vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	float dist = length(eyeCoord.xyz);
	float att = 500.0 / max(dist, 1.0);
	gl_PointSize = maxPointSize * att;
	gl_FrontColor = gl_Color;
}
