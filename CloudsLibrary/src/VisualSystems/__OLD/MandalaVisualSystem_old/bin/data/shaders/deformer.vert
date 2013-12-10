#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float time;
uniform float radius = 10.;
uniform vec2 radianOffset = vec2(0.,0.);

uniform int renderMode = 0;
uniform vec2 phiTheta = vec2(0.);

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

varying float isSide;



float PI = 3.14159265359;
float TWO_PI = 6.28318530718;
float epsilon = 0.0000001f;

varying float phi;
varying float theta;


float hash( vec2 p )
{
	float h = dot(p,vec2(127.1,311.7));
	
    return -1.0 + 2.0*fract(sin(h)*43758.5453123);
}

float noise2d( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );
	
	vec2 u = f*f*(3.0-2.0*f);
	
    return mix( mix( hash( i + vec2(0.0,0.0) ),
					hash( i + vec2(1.0,0.0) ), u.x),
			   mix( hash( i + vec2(0.0,1.0) ),
				   hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

vec3 mandala( float phi, float theta, float radius )
{
	return vec3( cos( phi ) * sin(theta), cos( theta ), sin( phi ) * sin( theta ) ) * radius;
}

void main()
{
	float xPos = gl_Vertex.x + radianOffset.x;
	float yPos = gl_Vertex.y + radianOffset.y;
	float zPos = gl_Vertex.z;
	
	//noise field
	if(renderMode == 0)
	{
		yPos += pow(noise2d( phiTheta * 10. + vec2(time*2.,0.)), 3.) + time * .25;
	}
	
	vec3 vertex;
	norm = gl_Normal;
	
	float rad = radius + zPos;
	vertex = mandala( xPos, yPos, rad );
	
	phi = xPos;
	theta = yPos;
	
	if( abs(norm.x) > .5 || abs(norm.y) > .5)
	{
		//transform the normal+vertex and use the difference to get our normal
		norm = gl_NormalMatrix * ( mandala( xPos+norm.x, yPos+norm.y, rad ) - vertex );
		isSide = 1.;
	}
	else
	{
		norm = gl_NormalMatrix * vertex * norm.z/abs(norm.z);
		isSide = 0.;
	}
	
	vec4 ecPosition = gl_ModelViewMatrix * vec4( vertex, 1. );
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
	uv = gl_MultiTexCoord0.xy;
}

