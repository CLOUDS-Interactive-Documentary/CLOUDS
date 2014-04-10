


uniform float percent;
varying float normPos;

void main(void)
{
	vec4 pos = gl_Vertex;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	gl_FrontColor = gl_Color;
	
	normPos = gl_Normal.x;


}
