
#version 120
#extension GL_ARB_texture_rectangle : enable

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
