
#version 120

uniform sampler2DRect position;
uniform sampler2DRect velocity;
uniform float dt;

vec4 getPosition() {
	return texture2DRect(position, gl_TexCoord[0].xy);
}

vec4 getVelocity() {
	return texture2DRect(velocity, gl_TexCoord[0].xy);
}

//this function is a force and returns velocity
void main() {
	gl_FragColor = getPosition() + getVelocity() * dt;
}