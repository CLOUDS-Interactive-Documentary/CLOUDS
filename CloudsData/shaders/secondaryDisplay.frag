
uniform float alphaAmt;

varying float normPos;
varying float filled;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), 0.0, 1.0);
}

void main(){
	
//	gl_FragColor = gl_Color;
//	gl_FragColor.a = alphaAmt * gl_Color.a;
	float expandedAlpha = map(normPos, 0.0, 1.0, .001, .999);
	float wipeValue = 1.- map(expandedAlpha, alphaAmt-.001, alphaAmt, 0., 1.0);
	float fadeValue = alphaAmt;
	gl_FragColor = vec4(gl_Color.rgb, mix(wipeValue,fadeValue,filled)*gl_Color.a);
}