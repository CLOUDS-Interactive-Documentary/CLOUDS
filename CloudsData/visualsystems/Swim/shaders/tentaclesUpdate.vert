#version 120
#extension GL_ARB_texture_rectangle : enable

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_Vertex;
}
