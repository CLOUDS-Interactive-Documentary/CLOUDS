#version 120
#extension GL_ARB_texture_rectangle : enable

#define TWO_PI 6.28318530717958647693
#define PI 3.14159265358979323846
#define HALF_PI  1.57079632679489661923

uniform sampler2D tex;

varying float fogAmount;
varying vec2 texOffset;

void main()
{
	gl_FragColor = gl_Color * (1.0 - fogAmount) * texture2D(tex, texOffset + 0.5 * gl_TexCoord[0].st);
}