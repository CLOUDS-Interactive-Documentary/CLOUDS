uniform vec2 resolution;
uniform vec2 mouse;
uniform float time;
uniform float zoom;

float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise2f( in vec2 p ){
	vec2 ip = vec2(floor(p));
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),  rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),   rand(ip+vec2(1.0,1.0)),u.x),
		u.y);
	return res*res;
}

float fbm(vec2 c) {
	float f = 0.0;
	float w = 1.0;
	for (int i = 0; i < 8; i++) {
		f+= w*noise2f(c);
		c*=2.0;
		w*=0.5;
	}
	return f;
}

vec2 cMul(vec2 a, vec2 b) {
	return vec2( a.x*b.x -  a.y*b.y,a.x*b.y + a.y * b.x);
}

float pattern(  vec2 p, out vec2 q, out vec2 r ){
	q.x = fbm( p  +0.1*time);
	q.y = fbm( p + vec2(1.0));
	r.x = fbm( p +1.0*q + vec2(1.7,9.2)+0.15*time );
	r.y = fbm( p+ 1.0*q + vec2(8.3,2.8)+0.126*time);
	return fbm(p +1.0*r + 0.0* time);
}

void main() {
	vec2 q;
	vec2 r;
	vec2 c = zoom*gl_FragCoord.xy/ resolution.xy;
	float f = pattern(c,q,r);
	gl_FragColor =  vec4(vec3(f),1.0);
}