#version 120

uniform float hoverPercent;
varying float ringPosition;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main() {
//    gl_FragColor = gl_Color;
	if(ringPosition < 0.0){
		gl_FragColor.rgb = mix(gl_Color.rgb, vec3(1.0), map(hoverPercent, 0.0, .05,0.,1.0));
	}
	else if(ringPosition < 1.0){
		//TODO illuminate segments
		gl_FragColor.rgb = mix(gl_Color.rgb, vec3(0.0, 0.0, 1.), map(hoverPercent, 0.0, .05,0.,1.0));
	}
	else{
		gl_FragColor.rgb = mix(gl_Color.rgb, vec3(.2), map(hoverPercent, 0.0, .05,0.,1.0) );
	}
	
	gl_FragColor.a = gl_Color.a;
	
//	if(ringPosition < 1.0){
//		gl_FragColor.r *= hoverPercent;
//	}
//	else if(ringPosition < 2.0){
//		gl_FragColor.g *= hoverPercent;
//	}
//	else if(ringPosition < 3.0){
//		gl_FragColor.b *= hoverPercent;
//	}
}