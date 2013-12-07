#version 120

uniform sampler2D tex;

void main(void)
{
	vec2 texCoord = gl_TexCoord[0].st;
	gl_FragColor = texture2D(tex, texCoord) * gl_Color;
}
