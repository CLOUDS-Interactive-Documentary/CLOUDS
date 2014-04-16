#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float shininess = 64.;
uniform float dim = 1000.;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

varying float zDist;

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = pow( abs(dot( ePos, normal )), shininess);
//	gl_FragColor = vec4( normal * .5 + .5 + fr, 1.) * color;
	gl_FragColor = vec4( color.xyz + fr, 1.);
	
	float atten = zDist / dim;
	gl_FragColor.xyz *= min(1., 1. - atten * atten * atten);
}

