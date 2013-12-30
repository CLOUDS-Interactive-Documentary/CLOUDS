
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float specularScale = 1.;
uniform float specularExpo = 64.;
uniform float discardThreshold = .35;

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main(void)
{
	float fr = dot( -normalize(ePos), normalize( norm ) ) ;
	
	if( abs(fr) > discardThreshold)	discard;
	fr *= specularScale * pow( fr* .5 + .5, specularExpo);
	
	gl_FragColor = vec4( gl_Color.xyz + fr, gl_Color.w );
}

