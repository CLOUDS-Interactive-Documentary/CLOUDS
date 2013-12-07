
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
	
//	if( fr > discardThreshold)	discard;
	fr *= specularScale * pow( fr* .5 + .5, specularExpo);
	
	gl_FragColor = vec4( gl_Color.xyz + fr, gl_Color.w );
	
	gl_FragColor.xyz = mix( vec3(0.), gl_FragColor.xyz, min(1., pow((1. - linearizeDepth( gl_FragCoord.z )) * falloffScl, falloffExpo) ) );
}

