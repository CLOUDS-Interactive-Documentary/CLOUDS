
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float specularScale = 1.;
uniform float specularExpo = 32.;

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main(void)
{
	vec3 normal = normalize( norm );
	float fr = dot( -normalize(ePos), normal ) ;
	fr *= specularScale * pow( fr* .5 + .5, specularExpo);
	
	gl_FragColor = vec4( normal * .5 + .5 + fr, 1.);
}

