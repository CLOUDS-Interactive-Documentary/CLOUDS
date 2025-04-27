
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex;

void main (void)
{
    vec4 test = texture2DRect(tex, gl_TexCoord[0].st);
	gl_FragColor = test;
}

