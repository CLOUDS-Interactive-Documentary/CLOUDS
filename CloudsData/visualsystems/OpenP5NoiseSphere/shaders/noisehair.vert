#version 110
#extension GL_ARB_texture_rectangle : enable

uniform float pointSize;
void main(void)
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_PointSize = pointSize;
	gl_FrontColor = gl_Color;
}
