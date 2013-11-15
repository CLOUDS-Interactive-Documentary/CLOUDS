#version 120

const float HALF_PI = 1.57079632679489661923;
const float PI = 3.14159265358979323846;
const float TWO_PI = 6.28318530717958647693;

//uniform float litAmt;
//uniform float innerGlowAmt;

uniform vec3 lightPos;
uniform float texAmount;
uniform float lightingAmount;
uniform vec4 colour;
uniform float texRepeatS;
uniform sampler2D tex;

varying vec3 nEye;
varying vec3 vEye;
//varying float superOffset;


void main()
{
	vec3 l = normalize(lightPos - vEye);
	
    vec3 n = normalize(nEye);
	
    // just diffuse + ambient
	float lighting = clamp(max(dot(n, l), 0.0) + 0.4, 0.0, 1.0);
	
    lighting = lightingAmount * lighting + 1.0 - lightingAmount;

    vec4 textured = texAmount * texture2D(tex, vec2(texRepeatS * gl_TexCoord[0].s, gl_TexCoord[0].t)) + (1.0 - texAmount);
    
    gl_FragColor = lighting * textured * colour;
}
