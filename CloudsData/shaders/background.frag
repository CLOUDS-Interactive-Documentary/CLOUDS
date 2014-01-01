#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect image;
uniform vec3 colorOne;
uniform vec3 colorTwo;
varying vec2 oTexCoord;

void main()
{
	gl_FragColor = vec4(mix(colorTwo,colorOne, texture2DRect(image,oTexCoord).r), 1.0);
}
