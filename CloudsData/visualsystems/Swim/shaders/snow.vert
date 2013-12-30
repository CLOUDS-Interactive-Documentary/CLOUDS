#version 120
#extension GL_ARB_texture_rectangle : enable 

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define HALF_PI  1.57079632679489661923

uniform float fogStart;
uniform float fogEnd;
uniform float nearClip;
uniform float camZ;
uniform float innerFogStart;
uniform float innerFogEnd;

varying float innerFogAmount;
varying float fogAmount;
varying vec2 texOffset;

void main()
{
    vec3 vWorld = gl_Vertex.xyz;
    vWorld.z = camZ - mod(camZ - vWorld.z, fogEnd);
    vec3 vEye = (gl_ModelViewMatrix * vec4(vWorld, 1.0)).xyz;
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    innerFogAmount = 1.0 - clamp((length(vEye) - innerFogStart) / (innerFogEnd - innerFogStart), 0.0, 1.0);
    texOffset = gl_Normal.xy;
    gl_FrontColor = gl_Color;
    gl_PointSize = nearClip * gl_Normal.z / -vEye.z;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ProjectionMatrix * vec4(vEye, 1.0);
}