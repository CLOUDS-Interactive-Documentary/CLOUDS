#version 120

uniform float minDistance;
uniform float maxDistance;

varying vec3 normPos;
varying float vertDistance;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main() {

	//normal debug system
//	gl_FragColor = vec4(normPos/vec3(100.) + vec3(.5),1.0);
	gl_FragColor = vec4(vec3( 1.0 - smoothstep(minDistance, maxDistance, vertDistance) ), 1.0);
}	