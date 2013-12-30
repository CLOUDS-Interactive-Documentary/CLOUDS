#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 c1;
uniform vec4 c2;
uniform float facingRatioExpo = 2.;

uniform vec4 specularColor = vec4(.5);

uniform vec4 fogColor = vec4(0.,0.,0.,1.);

uniform vec4 lightColor;
uniform float lightConstantAttenuation = .5;
uniform float lightLinearAttenuation = .1;
uniform float lightQuadraticAttenuation = .1;

varying vec3 lPos;

uniform float shininess;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec4 ecPosition;
varying vec2 uv;

varying float fogAmount;

const float LOG2 = 1.442695;

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
    attenuation = 1.0 / (lightConstantAttenuation + lightLinearAttenuation * d + lightQuadraticAttenuation * d * d);
	
    halfVector = normalize(VP + eye);

    nDotVP = abs( dot( normal, VP) );
    nDotHV = abs( dot( normal, halfVector) );

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, shininess);

    diffuse += lightColor * nDotVP * attenuation;
    specular += lightColor * pf * attenuation;
}

void main(void)
{
	vec3 normal = normalize(norm);
	
	float fr = abs( dot( ePos, -normal ));
	vec4 diffuse = mix( c1, c2, pow(fr,facingRatioExpo) ) * color;
	vec4 specular = vec4(vec3(specularColor), 0.);
	
	vec4 lightDiffuse = vec4(0.);
	vec4 lightSpecular = vec4(0.);
	
	PointLight( lPos, ePos, ecPosition.xyz, normal, lightDiffuse, lightSpecular, lightColor);
	
	gl_FragColor = diffuse * lightDiffuse + specular * lightSpecular;
	gl_FragColor = mix( vec4(fogColor.xyz, 0.), gl_FragColor, fogAmount);
}

