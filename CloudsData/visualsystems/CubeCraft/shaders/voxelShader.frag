
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 fogColor;
uniform float fogDist;
uniform float useFog;
uniform float fogExpo;

uniform float edgeSmoothing;
uniform vec4 edgeColor;
uniform vec4 fillColor;
uniform vec4 specularColor;

uniform float specExpo;
uniform float specScale;

uniform float drawCenters;
uniform float noiseCutoff;
uniform float cameraCutoffDistance;
uniform float edgeThreshold;

varying vec3 vertex;

varying float nScl;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

varying float camDelta;

float mapLinear( float x, float a1, float a2, float b1, float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

void main(void)
{
	//cull boxes based on noise and camera distance
	if( nScl < noiseCutoff)	discard;
	if( camDelta < cameraCutoffDistance*cameraCutoffDistance) discard;
	
	vec3 v = abs(vertex) * 2.;
	float edgeSample = max( v.x * v.y, max( v.y * v.z, v.z * v.x ));

	//edge color
	if(edgeSample > edgeThreshold)
	{
		gl_FragColor = edgeColor;
	}
	
	//fill color
	else
	{
		if(int(drawCenters) == 0)	discard;
		
		vec3 normal = normalize(norm);
		float fr = pow( abs(dot( ePos, normal )) * specScale, specExpo);
		
		gl_FragColor = mix( fillColor, specularColor, fr);
	}
	
	//smoothe edges
	float edgeMix = edgeSample + edgeSmoothing;
	if( edgeMix > edgeThreshold && edgeSample <= edgeThreshold)
	{
		gl_FragColor = mix( gl_FragColor, edgeColor, (edgeMix-edgeThreshold)/edgeSmoothing );
	}
	
	//fog
	if(useFog > .1)
	{
		gl_FragColor = mix( gl_FragColor, fogColor, min(1., pow( 1.25 * camDelta / (fogDist*fogDist), fogExpo) ) );
	}
}

