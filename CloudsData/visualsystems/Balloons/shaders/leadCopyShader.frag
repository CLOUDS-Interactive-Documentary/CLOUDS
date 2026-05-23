#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posTexture;

void main()
{
	gl_FragColor = vec4(texture2DRect(posTexture, vec2(0.0, 0.0)).xyz, 1.0);
}
