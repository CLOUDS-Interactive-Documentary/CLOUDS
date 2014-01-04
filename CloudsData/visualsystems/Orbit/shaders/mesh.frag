#version 120
#extension GL_ARB_texture_rectangle : enable

#define TWO_PI 6.28318530717958647693
#define PI 3.14159265358979323846
#define HALF_PI  1.57079632679489661923

uniform vec3 lEye;
uniform float litAmount;

varying float fogAmount;
varying vec3 nEye;
varying vec3 vEye;

void main()
{
    vec3 l = normalize(lEye - vEye);
	
    //vec3 lightN = normalize(cross(dFdx(vEye), dFdy(vEye)));
    vec3 lightN = normalize(nEye);
    
	float diffuse = max(dot(lightN, l), 0.0);
    
    vec3 h = normalize(lightN + l);
     
    float nDotH = max(dot(lightN, h), 0.0);
     
    float specular = 1e-2 * pow(nDotH, 64);
    
    gl_FragColor = (1.0 - fogAmount) * gl_Color * (1.0 - litAmount + litAmount * (diffuse + specular));
}