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
uniform float sampleColorWeight = .5;

uniform vec4 overallColor = vec4(1.);
varying vec4 col;
varying vec3 norm;
varying vec4 lPos;
varying vec3 vPos;
varying vec3 ePos;
varying vec4 ecPosition;
varying vec2 uv;
varying vec3 vertex;
varying vec4 position;

uniform float shininess = 4.;
uniform float radiusAlphaScl = 1.5;
uniform float facadeTextureAmount = .125;

uniform float bAlphaBlending = 1.;

uniform float bUseEdgeMap = 0.;


varying vec2 projImgUV;

uniform mat4 invProjection;

float toGreyScale( vec3 c ){
	return c.x * .3 + c.y*.59 + c.z * .11;
}

float maplinear(float x, float a1, float a2, float b1, float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

void main(void)
{
	float radiusAlpha = max(0., 1. - radiusAlphaScl * length(uv*2.-1.) );
	if(radiusAlpha < .001)	discard;
	
	if(int(drawEdges) == 1)
	{
		
		gl_FragColor = int(bUseEdgeMap) == 1 ? texture2DRect( projectedImage, projImgUV * projectedImageDim ) : vec4(1.);
		gl_FragColor *= vec4( vec3(1.), col.w * radiusAlpha) * gl_Color;
	}
	
	else
	{
		
		vec3 n = normalize( norm );
		vec3 diffuse, ambient, specular;
		
		vec3 VP = normalize( lPos.xyz - ecPosition.xyz );//no attenuation yet... so we don't need distance
		vec3 halfVector = normalize(VP + ePos);
		float nDotVP = max(0.0, dot(n, VP));
		float nDotHV = max(0.0, dot(n, halfVector));
		float fr = max(0.,dot( -normalize(ePos), normalize( norm ) ) );
		
		//lighting
//		ambient = gl_LightSource[0].ambient.xyz;
//		diffuse = gl_LightSource[0].diffuse.xyz * fr;//nDotVP; // temp solution for avoiding poorly positioned lights
//		float specVal = pow( fr, shininess);// pow(nDotVP, shininess);
//		specular = gl_LightSource[0].specular.xyz * specVal;
		
		diffuse = vec3( pow(fr*1.5, 2.) );
		float specVal = pow( fr, shininess);// pow(nDotVP, shininess);
		specular = gl_LightSource[0].specular.xyz * specVal;
		
		//super fake AO TODO: rename height gradient?
		float normalizedBuildingHeight = max( 0., (vertex.y - 1.) );
		normalizedBuildingHeight = min(1., max(0., maplinear(normalizedBuildingHeight, .0, 1., .4, 1.3 )));
		float superFakeAO = mix( 1., pow(normalizedBuildingHeight, superFakeAOExpo), superFakeAOAmount );
		
		vec3 sampleColor = mix( col.xyz, vec3(1.), sampleColorWeight);
		
		if(int(bAlphaBlending) == 1)
		{
			gl_FragColor = vec4( diffuse * superFakeAO * sampleColor + specular, col.w * radiusAlpha * sampleColor.x ) * overallColor;
		}
		else
		{
			gl_FragColor = vec4( diffuse * superFakeAO * sampleColor + specular, col.w * sampleColor.x ) * overallColor * radiusAlpha;
		}
	}
}

