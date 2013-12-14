#version 120

const float HALF_PI = 1.57079632679489661923;
const float PI = 3.14159265358979323846;
const float TWO_PI = 6.28318530717958647693;
const float EPSILON = 1e-4;

//uniform float size;
uniform float deformAmount;
uniform float time;
uniform float frequency;
uniform float fogStart;
uniform float fogEnd;

// eye space normal and light direction
varying vec3 nEye;
varying vec3 vEye;
varying float fogAmount;

vec4 deform(vec4 v)
{
    return vec4(v.x + sin(TWO_PI * gl_MultiTexCoord0.t + frequency * time) * gl_MultiTexCoord0.t * deformAmount * sin(HALF_PI + gl_MultiTexCoord0.s * TWO_PI),
                v.y - sin(TWO_PI * gl_MultiTexCoord0.t + frequency * time) * gl_MultiTexCoord0.t * deformAmount * cos(HALF_PI + gl_MultiTexCoord0.s * TWO_PI),
                v.z, 1.0);
}

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    vec4 vertex = deform(gl_Vertex);
    
    vec4 vertexEye = gl_ModelViewMatrix * vertex;
    
    vEye = vertexEye.xyz;
    
    nEye = gl_NormalMatrix * gl_Normal;
    
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    
    gl_Position = gl_ProjectionMatrix * vertexEye;
}