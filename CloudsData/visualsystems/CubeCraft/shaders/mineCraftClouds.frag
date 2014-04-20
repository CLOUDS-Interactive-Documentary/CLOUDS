
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 fogColor;
uniform float useFog;

uniform float edgeSmoothing;
uniform vec4 edgeColor;
uniform vec4 fillColor;
uniform vec4 specularColor;

uniform vec4 cloudShadowColor;
uniform float cloudShadow;

uniform float specExpo;
uniform float specScale;

uniform float drawCenters;
uniform float noiseCutoff;
uniform float edgeThreshold;

varying vec3 vertex;
varying vec3 vNormal;

varying float nScl;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

varying float camDelta;
varying float fogAmount;

varying float doDiscard;
varying float isCloud;


float mapLinear( float x, float a1, float a2, float b1, float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

void main(void)
{
	//cull boxes based on noise and camera distance
	if( doDiscard > .5)	discard;
	
	//normal rendering
	vec3 normal = normalize(norm);
	float fr = min(1., pow( max(0., dot( ePos, -normal )) * specScale, specExpo));
	
	gl_FragColor = vec4(1.);
	
	if( isCloud > .5)
	{
		gl_FragColor = mix(fogColor, vec4(fr), fr);// * (1. - fr*.1);
	}
	
	
	//fog
	if(useFog > .5)
	{
		gl_FragColor = mix( gl_FragColor, fogColor, fogAmount);
	}
}

