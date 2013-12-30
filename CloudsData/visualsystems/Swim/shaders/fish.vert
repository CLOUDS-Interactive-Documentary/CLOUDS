#version 120
#extension GL_ARB_texture_rectangle : enable 

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define HALF_PI  1.57079632679489661923

uniform float frequency;
uniform float time;
uniform vec3 lightPos;
uniform float size;
uniform vec3 deformAxis;
uniform float bend;
uniform float fogStart;
uniform float fogEnd;

//varying vec3 nEye;
varying vec3 vEye;
varying float fogAmount;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    float deformAmt = sin(time * frequency) * 40.0 * sin(HALF_PI + 0.75 * TWO_PI * gl_TexCoord[0].s / bend);
    
    vec3 vertex = size * gl_Vertex.xyz;
    vertex += deformAmt * deformAxis;
    vec4 vertexEye = gl_ModelViewMatrix * vec4(vertex.xyz, 1.0);
    vEye = vertexEye.xyz;
    
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    
    //vec4 xEps = deform(gl_Vertex + vec4(EPSILON, 0.0, 0.0, 0.0));
    //vec4 yEps = deform(gl_Vertex + vec4(0.0, EPSILON, 0.0, 0.0));
    //nEye = gl_NormalMatrix * twist(gl_Normal, twistAmount);//cross((xEps - vertex).xyz, (yEps - vertex).xyz);*/
    
    gl_Position = gl_ProjectionMatrix * vertexEye;
}