#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;

varying float positionValid;
varying vec3 normal;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample;

uniform float eyeMultiplier;
uniform float mouthMultiplier;
uniform float skinMultiplier;
uniform float baseMultiplier;

const float epsilon = 1e-6;


float isEye(){
	return (faceFeatureSample.r > epsilon && faceFeatureSample.b < .1) ? faceFeatureSample.r : 0.;
}

float isFace(){
	return (faceFeatureSample.g > epsilon && faceFeatureSample.b < .1) ? faceFeatureSample.g : 0.;
}

float isMouth(){
	return (faceFeatureSample.b > epsilon && faceFeatureSample.r < .1) ? faceFeatureSample.b : 0.;
}


float isSkin(){
	return faceFeatureSample.r > epsilon ||
			faceFeatureSample.g > epsilon ||
			faceFeatureSample.b > epsilon ?
			max( max(faceFeatureSample.r,faceFeatureSample.g),faceFeatureSample.b) : 0.;
}

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

	float attenuate = baseMultiplier + max(isEye() * eyeMultiplier,
										   max(isMouth() * mouthMultiplier,
											   isSkin() * skinMultiplier) );
	if(attenuate < epsilon){
		discard;
		return;
	}
	
    vec4 col = texture2DRect(texture, gl_TexCoord[0].st);
    gl_FragColor = gl_Color * col * attenuate;
}

