#version 120
uniform sampler2DRect map;
uniform vec4 startColor;
uniform vec4 endColor;

uniform float fogDistance = 1000.;
uniform float fogAttenuation = 2.;
uniform float fogExpo = 2.;
uniform vec4 fogColor = vec4(0.,0.,0.,1.);

uniform vec3 cameraPosition;

uniform vec4 fwColors[5];
uniform vec4 fwDeathColors[5];

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

float CubicInOut(float k){
	if ( ( k *= 2 ) < 1 ) return 0.5 * k * k * k;
	return 0.5 * ( ( k -= 2 ) * k * k + 2 );
}



float BounceOut(float k){
	if (k<(1./2.75)){
		return 7.5625 * k * k;
	}
	else if ( k < ( 2. / 2.75 ) ) {
		return 7.5625 * ( k -= ( 1.5 / 2.75 ) ) * k + 0.75;
	}
	else if ( k < ( 2.5 / 2.75 ) ) {
		return 7.5625 * ( k -= ( 2.25 / 2.75 ) ) * k + 0.9375;
	}
	else {
		return 7.5625 * ( k -= ( 2.625 / 2.75 ) ) * k + 0.984375;
	}
}


float BounceIn(float k){
	return 1. - BounceOut( 1. - k );
}
float BounceInOut(float k){
	if ( k < 0.5 ) return BounceIn( k * 2. ) * 0.5;
	return BounceOut( k * 2. - 1. ) * 0.5 + 0.5;
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
	float camDelta = length( ecPosition.xyz );
	attenuation = pow(max(0., 1. -  camDelta / 500.), 2.);
	pointSize = max( minPointSize, min( maxPointSize, particleSize * attenuation * (1. - age) ) );
	gl_PointSize = pointSize;
	
	//color
	int colorIndex = int(gl_Color.a);
	color = mix( fwColors[colorIndex], fwDeathColors[colorIndex], ma );
	
	float fogAmount = 1. - pow(1.25 * max(0., min(1., camDelta / fogDistance )), fogExpo) * fogAttenuation;
	
	color = mix( vec4(fogColor.xyz, 0.), color, fogAmount);// pow( (1. - camDelta / fogDistance), fogExpo) * fogAttenuation );
	
	//rotation
	float angle = rotationRate * (birthTime + pos.x + pos.y + pos.z);
	q.x = 0.0f;
	q.y = 0.0f;
	q.z = sin(angle / 2.);
	q.w = cos(angle / 2.);
	
	//texture index
	tIndex = gl_Color.b;// mod( birthTime*1., 4.);
}
