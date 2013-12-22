#version 120
#extension GL_ARB_texture_rectangle : enable 

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define HALF_PI  1.57079632679489661923

uniform float fogStart;
uniform float fogEnd;

varying float fogAmount;
varying vec3 vEye;
varying vec3 nEye;

void main()
{
    vEye = (gl_ModelViewMatrix * gl_Vertex).xyz;
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    nEye = gl_NormalMatrix * gl_Normal;
    
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}