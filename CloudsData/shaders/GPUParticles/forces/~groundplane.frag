
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
	vec4 p = getPosition();
	vec4 v = getVelocity();

	//if (p.y > 0)
		gl_FragColor = p;
	//else
	//	gl_FragColor = vec4(p.x, 0, p.z);
}