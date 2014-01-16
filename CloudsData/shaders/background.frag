#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect image;
uniform vec3 colorOne;
uniform vec3 colorTwo;
uniform float gradientExponent;
varying vec2 oTexCoord;

void main()
{
	gl_FragColor = vec4( mix(colorTwo,colorOne, pow(texture2DRect(image,oTexCoord).r, gradientExponent)), 1.0);
}
