
#version 120

uniform sampler2DRect position;
uniform sampler2DRect velocity;

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
	//velocity.y = 0.01;
	gl_FragColor = velocity;
}