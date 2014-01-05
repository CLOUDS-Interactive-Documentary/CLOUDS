
uniform float distanceMin;
uniform float distanceMax;

varying float normalizedDist;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main() {
//	distanceMin
	float distanceAttenuate = pow(1. - normalizedDist,4.);
//	gl_FragColor = gl_Color * distanceMin;
	gl_FragColor = gl_Color;
	gl_FragColor.a = distanceAttenuate;
//	gl_FragColor.rgb = vec3(distanceAttenuate);
}