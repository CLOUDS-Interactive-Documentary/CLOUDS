
#version 120

uniform sampler2DRect position;
uniform sampler2DRect velocity;

uniform float t;
uniform float dt;

vec4 getPosition() {
	return texture2DRect(position, gl_TexCoord[0].xy);
}

vec4 getVelocity() {
	return texture2DRect(velocity, gl_TexCoord[0].xy);
}

//this function is a force and returns velocity
void main() {
	vec4 position = getPosition();
	vec4 velocity = getVelocity();
	//velocity += 0.00;//1;
	gl_FragColor = vec4(0,0.1,0,0);
}