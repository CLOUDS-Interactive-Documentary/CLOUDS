#version 120
#extension GL_ARB_texture_rectangle : enable

#define TWO_PI 6.28318530717958647693
#define PI 3.14159265358979323846
#define HALF_PI  1.57079632679489661923

uniform vec3 lightPos;
uniform float time;
uniform float frequency;
uniform vec3 colour;
uniform sampler2D tex;
uniform float texAmt;

//varying vec3 nEye;
varying vec3 vEye;
varying float fogAmount;

void main()
{
    gl_FragColor = (1.0 - fogAmount) * vec4(colour.rgb * texture2D(tex, gl_TexCoord[0].st).rgb, 1.0);
}