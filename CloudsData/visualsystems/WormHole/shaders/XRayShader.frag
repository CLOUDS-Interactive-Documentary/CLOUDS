
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 c1;
uniform vec4 c2;

uniform float specularScale = 1.2;
uniform float shininess = 32.;
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
	
	float xray = pow(specularScale - fr, max( .51, shininess * .1));
	
	gl_FragColor = vec4( gl_Color.xyz * xray, fr) * mix(c1, c2, xray);
}

