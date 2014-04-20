
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 fogColor;
uniform float useFog;

uniform float groundDrama;
uniform float maxHeight = 10.;

uniform vec4 groundColor = vec4( .4, 1., .6, 1.);
uniform vec4 undergroundColor = vec4( .6 ,.2, .1, 1.);
uniform vec4 cloudShadowColor;
uniform float cloudShadow;

uniform float specExpo;
uniform float specScale;

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

varying vec4 groundSample;
varying float underSky;

varying float lowerBoundY;
varying float upperBoundY;


float mapLinear( float x, float a1, float a2, float b1, float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

void main(void)
{
	//normal rendering
	vec3 normal = normalize(norm);
	float fr = pow( max(0.,dot( ePos, -normal )) * specScale, specExpo);
	
	float vMix = max(0., mapLinear(vertex.y, lowerBoundY, upperBoundY, 0., 1. ));
	
	vec4 topColor = mix( undergroundColor, groundColor, pow(1.2*vertex.y/(maxHeight*groundDrama),2.) );
	if(vNormal.y > .5)
	{
		gl_FragColor = topColor;//mix( undergroundColor, groundColor, pow(1.2*vertex.y/(maxHeight*groundDrama),2.) );// * (groundSample*.75+.5);
	}
	else{
		gl_FragColor = mix(undergroundColor, topColor, pow(vMix, 2.) );
	}
	
	if(underSky>.5){
		gl_FragColor = mix( gl_FragColor, cloudShadowColor, cloudShadow);
	}
	
	//fog
	if(useFog > .5)
	{
		gl_FragColor = mix( gl_FragColor, fogColor, fogAmount );
	}
}

