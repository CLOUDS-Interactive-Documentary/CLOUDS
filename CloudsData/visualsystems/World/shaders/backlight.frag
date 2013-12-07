
#version 120

varying vec3 eyeVec;

varying float angle;

varying vec3 vNorm;

//this function is a force and returns velocity
void main() {
	
	//velocity.y = 0.01;
	//gl_FragColor = vec4(vNorm * .5 + 1.0, 1.0);
	
	if(angle > 0.0){
		gl_FragColor = gl_Color * vec4( vec3( pow(1.-angle, 4.0) / 1.57079632679), 1.0);
	}
	else {
		gl_FragColor = vec4(0.0);
	}
}