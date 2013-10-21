
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float specularScale = 1.;
uniform float specularExpo = 64.;
uniform float discardThreshold = .35;

uniform float falloffDist =  2000.;
uniform float falloffExpo = 2.;
uniform float falloffScl = 1.2;

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

float nearClip = 1.;

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (falloffDist + nearClip - d * (falloffDist - nearClip));
}

void main(void)
{
	float fr = abs(dot( -normalize(ePos), normalize( norm ) ) );
	
	float xray = pow(1. - fr, max( .51, specularExpo * .1));
	
	gl_FragColor = vec4( gl_Color.xyz * xray, fr);
}

