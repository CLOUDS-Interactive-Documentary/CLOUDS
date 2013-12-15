#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float shininess;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = pow( abs(dot( ePos, normal )), shininess);
	gl_FragColor = vec4( normal * .5 + .5 + fr, 1.) * color;
//	gl_FragColor = vec4( uv + fr, 1., 1.);
}

