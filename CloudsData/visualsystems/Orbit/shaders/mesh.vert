#version 120
#extension GL_ARB_texture_rectangle : enable 

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define HALF_PI  1.57079632679489661923

uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform float fogStart;
uniform float fogEnd;

varying float fogAmount;
varying vec3 vEye;
varying vec3 nEye;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vec4 vWorld = vec4(texture2DRect(particles0, gl_TexCoord[0].st).xyz, 1.0);
    vec3 nWorld = texture2DRect(particles1, gl_TexCoord[0].st).xyz;
    
    vEye = (gl_ModelViewMatrix * vWorld).xyz;
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    nEye = gl_NormalMatrix * nWorld;
    
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vWorld;
}