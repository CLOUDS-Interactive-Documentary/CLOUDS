#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float shininess;
uniform float dim;
uniform float facingRatio;

uniform float lightConstantAttenuation = 1.;
uniform float lightLinearAttenuation = .0025;

varying vec4 lPos;
varying vec4 lCol;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec4 ecPosition;

varying float fogMix;

void PointLight(in vec3 lightPosition,
                in vec3 eye,
                in vec3 ecPosition3,
                in vec3 normal,
                inout vec4 diffuse,
                inout vec4 specular,
				in vec4 lightColor
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
    attenuation = 1.0 / (lightConstantAttenuation + lightLinearAttenuation * d);
	
    nDotVP = pow( max(0., dot( normal, VP) ), shininess);
    diffuse += lightColor * nDotVP * attenuation;
}

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = abs(dot(ePos, normal));
//	float spec = pow(fr, shininess);
	
//	fr  = mix(1., fr * 1.2, facingRatio);
	
	vec4 diffuse = color*mix(1., fr * 1.2, facingRatio);
	vec4 specular = vec4(1.);
	
	PointLight( lPos.xyz, ePos, ecPosition.xyz, normal, diffuse, specular, lCol);
	
//	float atten = zDist / dim;
//	gl_FragColor.xyz *= min(1., 1. - atten * atten * atten);
	
	gl_FragColor = diffuse;// * specular;
	gl_FragColor.xyz *= fogMix;
	gl_FragColor.w = 1. - fr * .025;
}

