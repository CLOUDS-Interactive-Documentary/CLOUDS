#version 120

uniform float hoverPercent;
uniform int drawingTimer;

uniform int selected;
uniform float selectionAnimationPercent;
uniform float maxRingPosition;

varying float ringPosition;
varying float timerGeoFlag; //0 if it is a ring, 1 if it is a timer
const float epsilon = 1e-6;
varying float segmentPosition;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), inputMin, inputMax);
}

void main() {

	//default hide everything
	gl_FragColor.a = 0.0;
	
	//this is a timer segment in the timer
	if(drawingTimer == 1 && timerGeoFlag > epsilon){
		//highlight based on the segment position so it looks like a timer
		float hoverthresh = .05;
		float startDelay  = .05;
		float hoverClamp = map(hoverPercent, 0.0, 1.0, 0.0, 1.-hoverthresh-startDelay);
		float hoverfade = 1.0 - smoothstep(hoverClamp, hoverPercent+startDelay, 1.0 - segmentPosition - hoverthresh);
		//also fade in a little bit at the beginning
		hoverfade *= smoothstep(0.0, startDelay, hoverPercent);
		//if we are the inner ring be white
		if(ringPosition < .5){
			gl_FragColor.rgb = vec3(1.);
		}
		//othwerise take the normal color
		else{
			gl_FragColor.rgb = gl_Color.rgb;
		}
		//attenuate based on the color
		gl_FragColor.a = gl_Color.a*hoverfade;
		
	}
	else if(selected == 1 && ringPosition > 3.0-epsilon){
//		float ringRevealStep = selectionAnimationPercent;
		float ringRevealStep = ( (ringPosition-3.0) / (maxRingPosition-3.0) );
		float selectReveal = smoothstep(ringRevealStep, ringRevealStep + .1, selectionAnimationPercent);
		gl_FragColor.rgb = gl_Color.rgb;
		gl_FragColor.a = gl_Color.a*selectReveal;
	}
	else if(ringPosition < 3.0-epsilon){
		//normal, just draw based on the hover percent
		float hoverfade = 1.0 - smoothstep(0.0, .3, hoverPercent);
		gl_FragColor.rgb = gl_Color.rgb;
		gl_FragColor.a = gl_Color.a*hoverfade;
		
	}

//	gl_FragColor.rgb = vec3();
//	gl_FragColor.a = gl_Color.a;
	//TURN TIMERS RED TEST
//	if(timerGeoFlag > .8){
//		gl_FragColor.rgb = vec3(hoverfade, 0.0, 0.0);
//	}
//	if(ringPosition < 0.0){
//	}
//	//middle ring
//	else if(ringPosition < 1.0){
//	}
//	else{
//	}
	
	
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