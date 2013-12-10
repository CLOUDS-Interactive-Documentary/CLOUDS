
#version 120
#extension GL_ARB_texture_rectangle : enable


uniform sampler2DRect frontTexture;
uniform int usingFrontTexture = 0;
uniform vec2 frontTexDim;

uniform sampler2DRect sideTexture;
uniform int usingSideTexture = 0;
uniform vec2 sideTexDim;

uniform vec4 frontColor = vec4(1.);
uniform vec4 sideColor = vec4(1.);

uniform float diffuseScale = .5;
uniform float specularScale = .75;
uniform float specularExpo = 32.;

uniform vec2 sweep;
uniform vec2 startSweep;

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
	float fr = dot( -normalize(ePos), normal );
	
	vec4 diffuse = vec4(1.);
	vec4 specular = vec4( vec3(specularScale * pow( abs(fr), specularExpo) ), 0.);
	
	//side faces
	if(isSide > .9)
	{
		//diffuse
		diffuse *= sideColor;
		
		//texture
		if(usingSideTexture > .9)
		{
			diffuse *= texture2DRect( sideTexture, uv * sideTexDim );
		}
	}
	//front and back faces
	else
	{
		//diffuse color
		diffuse *= frontColor;
		
		//texture
		if(usingFrontTexture > .9)
		{
			diffuse *= texture2DRect( frontTexture, uv * frontTexDim );
		}
	}
	
	//diffuse
	diffuse.xyz *= fr * (1.-diffuseScale) + diffuseScale;
	
	//add it all up
	gl_FragColor = diffuse + specular;
	
	//
	if(gl_FragColor.w < discardThreshold)	discard;
}

