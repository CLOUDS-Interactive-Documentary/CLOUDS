#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;

varying float positionValid;
varying vec3 normal;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample;

uniform float eyeMultiplier;
uniform float skinMultiplier;
uniform float baseMultiplier;

const float epsilon = 1e-6;


float isEye(){
	return min(min(faceFeatureSample.r, faceFeatureSample.g), faceFeatureSample.b);
}

float isFace(){
	return min(faceFeatureSample.r, faceFeatureSample.g);
}

float isSkin(){
	return min(faceFeatureSample.r + faceFeatureSample.g, 1.0);
}

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

	float attenuate = baseMultiplier + max(isEye() * eyeMultiplier, isSkin() * skinMultiplier);
	if(attenuate < epsilon){
		discard;
		return;
	}
	
    vec4 col = texture2DRect(texture, gl_TexCoord[0].st);
    gl_FragColor = gl_Color * col * attenuate;
}

