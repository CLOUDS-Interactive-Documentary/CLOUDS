
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float specularScale = 1.;
uniform float specularExpo = 32.;

uniform vec2 sweep;
uniform vec2 startSweep;

uniform vec3 sideColor = vec3( 1., .6, .1);
uniform float discardThreshold = .005;

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;
varying float isSide;
varying float phi;
varying float theta;


float PI = 3.14159265359;
float TWO_PI = 6.28318530718;
float epsilon = 0.0000001f;

void main(void)
{
	vec3 normal = normalize( norm );
	float fr = dot( -normalize(ePos), normal ) ;
	fr *= specularScale * pow( fr* .5 + .5, specularExpo);
		
	
	gl_FragColor = vec4( normal * .5 + .5 + fr, 1.);
//	if(isSide > .9)
//	{
//		
//	}else{
//		gl_FragColor *= pow(abs(sin( (phi * 4. * sweep.x + startSweep.x) * TWO_PI))*1.1, 32.);
//	}
	
	//
//	if(gl_FragColor.w < discardThreshold)	discard;
}

