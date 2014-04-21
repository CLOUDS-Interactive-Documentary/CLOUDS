#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float shininess = 3.;
uniform float dim;
uniform float facingRatio = .4;

uniform sampler2DRect sphericalMap;

uniform float screenHeight;
uniform vec4 bg0;
uniform vec4 bg1;
uniform float bgExpo;

varying vec4 lPos;
varying vec4 lCol;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec4 ecPosition;

varying float fogMix;
varying vec2 vN;

varying vec4 wordLightPos;
varying float wordLightAtten;

varying vec4 wordLightPos0;
varying float wordLightAtten0;


varying float creditLight;

uniform vec2 sphericalMapDim;

void PointLight(in vec3 lightPosition,
                in vec3 eye,
                in vec3 ecPosition3,
                in vec3 normal,
                inout vec4 diffuse,
                inout vec4 specular,
				in vec4 lightColor,
				in float linearAtten,
				in float quadAtten,
				in float shine
				)
{
    float nDotVP, nDotHV, pf, attenuation, d;
    vec3  VP, halfVector;
	
    // Compute vector from surface to light position
    VP = lightPosition - ecPosition3;
	
    // Compute distance between surface and light position
    d = length(VP);
	
    // Normalize the vector from surface to light position
    VP = normalize(VP);
	
    // Compute attenuation.  default is 1, 0, 0 which is no attenutation
    attenuation = 1.0 / (1.	+ linearAtten * d + quadAtten * d * d);
	
    nDotVP = pow( dot( normal, VP)*.5 + .5, shine);
	//nDotVP = pow( max(0., dot( normal, VP) ), shine);
    diffuse += 1.1 * lightColor * nDotVP * attenuation;
}

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = abs(dot(ePos, normal));
	
	vec4 diffuse = color * mix(1., fr, facingRatio);
	vec4 specular = vec4(1.);
	
	PointLight( lPos.xyz, ePos, ecPosition.xyz, normal, diffuse, specular, lCol, 0., 0., shininess);
	
	diffuse += creditLight;

	vec4 fogColor = mix( bg1, bg0, pow(gl_FragCoord.y/screenHeight, bgExpo) );
	
	diffuse += texture2DRect( sphericalMap, vN ) * .25;
	
	gl_FragColor = mix( fogColor, diffuse, clamp(fogMix,0.,1.));
	gl_FragColor.w = 1. - fr * .05;
}

