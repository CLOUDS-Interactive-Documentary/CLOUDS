uniform float percentOptionsRevealed;
uniform int fadeIn;
varying float optionPosition;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin,outputMax);
}

void main () {
	float optionsExponent = pow(percentOptionsRevealed,10.);
	float optionsReveal;
	if(fadeIn == 1){
		optionsReveal = 1. - map(optionPosition, optionsExponent-.01, optionsExponent, 0.0, 1.0);
	}
	else{
		optionsReveal = map(1. - optionPosition, optionsExponent, optionsExponent+.05, 0.0, 1.0);
	}
	
	gl_FragColor.rgb = gl_Color.rgb * optionsReveal * 1.0;
	gl_FragColor.a = 1.0;
	
	gl_FragColor.rgb = vec3(gl_Color.r*optionsReveal);
}
