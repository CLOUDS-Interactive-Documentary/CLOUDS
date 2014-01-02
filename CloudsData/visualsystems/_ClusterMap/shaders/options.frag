uniform vec4 lineColor;
uniform float percentOptionsRevealed;
uniform int fadeIn;
varying float optionPosition;
varying float handleAttenutation;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin,outputMax);
}

void main () {
	float optionsExponent = percentOptionsRevealed;
	float optionsReveal;
	if(fadeIn == 1){
		optionsReveal = 1.- map(1.-optionPosition, optionsExponent-.2, optionsExponent+.2, 0.0, 1.0);
	}
	else{
		optionsReveal = map(1. - optionPosition, optionsExponent-.2, optionsExponent+.05, 0.0, 1.0);
	}
	
	gl_FragColor.rgb = lineColor.rgb * lineColor.a * optionsReveal * handleAttenutation;
	gl_FragColor.a = 1.0;
	
//	gl_FragColor.rgb = vec3(gl_Color.r*optionsReveal);
}
