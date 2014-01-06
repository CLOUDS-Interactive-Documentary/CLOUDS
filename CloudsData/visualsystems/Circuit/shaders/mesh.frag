
uniform float distanceMin;
uniform float distanceMax;
varying float normalizedDist;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main() {
	float distanceAttenuate = pow(1. - normalizedDist,4.);
	gl_FragColor.rgb = gl_Color.rgb * distanceAttenuate*gl_Color.a;
	gl_FragColor.a = 1.0;
//	gl_FragColor.rgb = vec3(distanceAttenuate);
}