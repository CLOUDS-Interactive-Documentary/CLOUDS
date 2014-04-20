
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float groundDrama = .75;

uniform float cloudThreshold = .55;
uniform float cloudHeight = 4.;
uniform float cloudThickness = 8.;

uniform vec3 cubeScale = vec3(1.,1.,1.);

uniform vec3 cameraPos;
uniform float cameraCutoffDistance;

uniform vec3 noiseOffset;
uniform vec3 cameraOffset;

uniform float maxHeight = 10.;


uniform float fogDist;
uniform float fogExpo;

varying vec3 vertex;
varying vec3 vNormal;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

varying float camDelta;
varying float fogAmount;

varying float doDiscard = .0;
varying vec4 groundSample;
varying float underSky;
varying float lowerBoundY;
varying float upperBoundY;

vec3 mod289(vec3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
	return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
											// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	
	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	
	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
					 + i.x + vec3(0.0, i1.x, 1.0 ));
	
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	
	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
	
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	
	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	
	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float mapLinear( float x, float a1, float a2, float b1, float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

float lengthSqr(vec3 x)
{
	return dot(x,x);
}

float sampleGroundNoise( vec2 p )
{
	return snoise( p * .05 + vec2(300., 127.)) * snoise( p * .025 + vec2(300., 127.)) * snoise( p * .01) * .5 + .5;
}

float sampleSkyNoise( vec2 p )
{
	return snoise( p * .1 + vec2(5100.+noiseOffset.x, 227.+noiseOffset.z)) * snoise( p * .05 ) * snoise( p * .01 + 1.) * .5 + .5;
}


float CubicInOut(float k){
	if ( ( k *= 2 ) < 1 ) return 0.5 * k * k * k;
	return 0.5 * ( ( k -= 2 ) * k * k + 2 );
}

void main()
{
	doDiscard = 0.;
	
	//important variables
	vertex = gl_Vertex.xyz;
	
	uv = gl_MultiTexCoord0.xy;
	
	vNormal = gl_Normal;
	norm = gl_NormalMatrix * gl_Normal;
	
	vec3 cd = vec3( fract(cameraOffset.x), 0., fract(cameraOffset.z) );
	
	vec3 boxCenter = gl_Color.xyz;
	
	if( camDelta < cameraCutoffDistance*cameraCutoffDistance)
	{
		doDiscard = 1.;
	}
	
	vec4 v = gl_Vertex;
	v.xz -= fract(cameraOffset.xz);
	
	
	//ground
	vec2 boxXZ = boxCenter.xz + floor( cameraOffset.xz );
	
	float xzNoise = CubicInOut(sampleGroundNoise( boxXZ )) * groundDrama;
	
	
	upperBoundY = floor(xzNoise * maxHeight) + .5;
	lowerBoundY = upperBoundY - 1.;
	
	if(v.y > 0.1)
	{
		v.y += floor(xzNoise * maxHeight);
		
		//v.xz *= .9;
	}
	vertex = v.xyz;
	
	v.xyz *= cubeScale;
	
	groundSample = vec4( pow(xzNoise, 1.25) );
	
	underSky = sampleSkyNoise( boxXZ ) > cloudThreshold ? 1. : 0.;
	
	//output the vertex
	vec4 ecPosition = gl_ModelViewMatrix * (v + vec4(boxCenter, 0.) );
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
//	camDelta = length(cameraPos - boxCenter + cd);
	
	camDelta = length( ecPosition.xyz );
	fogAmount = min(1., max(0., 1.25 * pow( (camDelta / (fogDist*30.)), fogExpo) ) );
}

