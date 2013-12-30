#version 120

varying float normalizedDist;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main() {

	gl_FragColor = gl_Color;
	gl_FragColor.a = 1.- pow(normalizedDist,2.0);
	
}	