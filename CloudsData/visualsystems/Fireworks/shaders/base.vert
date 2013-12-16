#version 120
uniform sampler2DRect map;
uniform vec4 startColor;
uniform vec4 endColor;

uniform float fogDist = 1000.;
uniform float fogAttenuation = 2.;
uniform float fogExpo = 2.;
uniform vec4 fogColor = vec4(0.,0.,0.,1.);

uniform vec3 cameraPosition;

uniform float rotationRate = 1.;
uniform float nearClip;
uniform float farClip;

uniform float maxPointSize = 25.;
uniform float minPointSize = 0;
uniform float time = 0.;
uniform float frameRate = .016;
uniform float particleSize = 10.;

uniform vec3 gravity = vec3( 0., -98., 0. );

uniform float speed = 10.;

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


	
float CubicIn( float k ) {
	return k * k * k;
}
float CubicOut( float k ) {
	return --k * k * k + 1;
}
	
float QuarticIn(float k) {
	return k * k * k * k;
}
float QuarticOut(float k) {
	return 1 - ( --k * k * k * k );
}
	
float QuinticIn ( float k ) {
	return k * k * k * k * k;
}
float QuinticOut (float k ) {
	return --k * k * k * k * k + 1;
}

void main(){
	
	//life and death
	float birthTime = gl_Color.r;
	float lifeSpan = gl_Color.g / speed;

	age = min( 1., (time - birthTime) / lifeSpan );
	
	//velocity
	vec3 vel = gl_Normal.xyz;
	float ma = CubicOut( age );
	vec3 posOffset = vel * ma;
//	posOffset += gravity * ma;
	
	//position
	vec4 pos = gl_Vertex + vec4(posOffset, 0.);
	vec4  ecPosition = gl_ModelViewMatrix * pos;
	ecPosition3 = ecPosition.xyz / ecPosition.w;
	gl_Position = gl_ProjectionMatrix * ecPosition;
	eye = -normalize( ecPosition3 );
	
	//point size
	float camDelta = length( ecPosition );
	attenuation = max(0., 1. -  camDelta / 400.);
	pointSize = max( minPointSize, min( maxPointSize, particleSize * attenuation * (1. - age) ) );
	gl_PointSize = pointSize;
	
	//color
	color = mix( startColor, endColor, pow(ma, 3.) );// age );
	
	color = mix( fogColor, color, pow( (1. - camDelta / fogDist), fogExpo) * fogAttenuation );
	
	//rotation
	float angle = rotationRate * (birthTime + pos.x + pos.y + pos.z);
	q.x = 0.0f;
	q.y = 0.0f;
	q.z = sin(angle / 2.);
	q.w = cos(angle / 2.);
	
	//texture index
	tIndex = mod( birthTime*1., 3.);
}
