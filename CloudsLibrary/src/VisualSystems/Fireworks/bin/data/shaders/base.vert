#version 120
uniform sampler2DRect map;
uniform vec4 startColor;
uniform vec4 endColor;
uniform float rotationRate = 1.;
uniform float nearClip;
uniform float farClip;

uniform float maxPointSize = 25.;
uniform float minPointSize = 0;
uniform float time = 0.;
uniform float frameRate = .016;
uniform float size = 10.;

uniform vec3 gravity = vec3( 0., -98., 0. );
uniform vec3 cameraPosition;

varying vec3 ecPosition3;
varying vec3 eye;
varying vec4 color;
varying float age;
varying float pointSize;
varying float attenuation;

varying vec4 q;

varying float tIndex;


float PI = 3.14159265359;
float HALF_PI = 1.57079632679;


void main(){
	
	//life and death
	float birthTime = gl_Color.r;
	float lifeSpan = gl_Color.g;

	float ageSec = (time - birthTime);
	age = min( 1., ageSec / lifeSpan );
	
	//velocity
	vec3 vel = gl_Normal.xyz;
	float ma = sin( age * age * HALF_PI );
	vec3 posOffset = vel * ma;
	posOffset += gravity * ma;
	
	//position
	vec4 pos = gl_Vertex + vec4(posOffset, 0.);
	vec4  ecPosition = gl_ModelViewMatrix * pos;
	ecPosition3 = ecPosition.xyz / ecPosition.w;
	gl_Position = gl_ProjectionMatrix * ecPosition;
	eye = -normalize( ecPosition3 );
	
	//point size
//	float attenuation = 1. - pow( max( 0.,min( 1., (length( ecPosition ) + 300.)/ 10000.)), 2.);//1000. / distance(pos.xyz, cameraPosition);
	attenuation =  100. / length( ecPosition );
	pointSize = max( minPointSize, min( maxPointSize, size * attenuation * (1. - age) ) );
	gl_PointSize = pointSize;
	
	//color
	color = mix( startColor, endColor, age );
	
	//rotation
	float angle = rotationRate * (birthTime + pos.x + pos.y + pos.z);
	q.x = 0.0f;
	q.y = 0.0f;
	q.z = sin(angle / 2.);
	q.w = cos(angle / 2.);
	
	//texture index
	tIndex = mod( birthTime*1., 3.);
	
//	if( mod( birthTime*100., 1 ) == 1. ){
//		gl_PointSize += 10. * (age + age);
//	}
	
}
