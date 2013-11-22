#version 120
#extension GL_ARB_texture_rectangle : enable 

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define HALF_PI  1.57079632679489661923

varying vec3 nEye;
varying vec3 vEye;

void main()
{
    vEye = (gl_ModelViewMatrix * gl_Vertex).xyz;
    nEye = gl_NormalMatrix * gl_Normal;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}