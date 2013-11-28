#version 120
#extension GL_ARB_texture_rectangle : enable

//uniform sampler2DRect pong;
//uniform float randSeed;
uniform sampler2DRect projectedImage;
uniform vec2 projectedImageDim;

uniform sampler2DRect facadeTexture;
uniform vec2 facadeTextureDim;
uniform float maxHeight;
uniform float drawEdges = 0.;
uniform float superFakeAOAmount = .4;
uniform float superFakeAOExpo = 1.;

varying vec4 col;
varying vec3 norm;
varying vec4 lPos;
varying vec3 vPos;
varying vec3 ePos;
varying vec4 ecPosition;
varying vec2 uv;
varying vec3 vertex;
varying vec4 position;

uniform float shininess = 32.;
uniform float radiusAlphaScl = 1.5;
uniform float facadeTextureAmount = .125;

varying vec2 projImgUV;

uniform mat4 invProjection;

float toGreyScale( vec3 c ){
	return c.x * .3 + c.y*.59 + c.z * .11;
}

void main(void)
{
	float radiusAlpha = min(1., 1.5 - pow( radiusAlphaScl * length(abs(uv*2.-1.)), 2.) );
	if(radiusAlpha < .001)	discard;
	
	if(int(drawEdges) == 1)
	{	
		gl_FragColor = texture2DRect( projectedImage, projImgUV * projectedImageDim );
		gl_FragColor *= vec4( col.xyz, col.w * radiusAlpha) * gl_Color;
	}
	else
	{
		
		vec3 n = normalize( norm );
		vec3 diffuse, ambient, specular;
		
		vec3 VP = normalize( lPos.xyz - ecPosition.xyz );//no attenuation (yet...) so we don't need distance
		vec3 halfVector = normalize(VP + ePos);
		float nDotVP = max(0.0, dot(n, VP));
		float nDotHV = max(0.0, dot(n, halfVector));
		float fr = max(0.,dot( -normalize(ePos), normalize( norm ) ) );
		
		//lighting
		ambient = gl_LightSource[0].ambient.xyz;
		diffuse = gl_LightSource[0].diffuse.xyz * fr;//nDotVP; // temp solution for avoiding poorly positioned lights
		float specVal = pow(nDotVP, shininess);
		specular = gl_LightSource[0].specular.xyz * specVal;
		
		//super fake AO
		diffuse *= pow(min(1., 1.1 * vertex.y ), superFakeAOExpo ) * superFakeAOAmount + (1. - superFakeAOAmount);
		
		gl_FragColor = vec4( (diffuse + ambient + specular)*col.xyz, col.w * radiusAlpha);// * gl_Color;
	}
}

