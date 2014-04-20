
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float halfGridDim = 100.;
uniform float falloffDist =  2000.;
uniform float falloffExpo = 2.;
uniform float falloffScl = 1.2;
uniform float alphaScale = 1.;

varying vec3 ePos;
varying vec2 endVal;

varying float dist;

void main(void)
{
	gl_FragColor = mix( gl_Color, vec4(gl_Color.xyz, 0.), min(1., pow( falloffExpo * dist /falloffDist, falloffExpo)));
}