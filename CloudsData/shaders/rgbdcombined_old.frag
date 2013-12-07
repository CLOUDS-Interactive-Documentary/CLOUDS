#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect rgbdTexture;

varying float positionValid;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample;


//LIGHTING
varying vec3 eye;
varying vec3 normal;
varying float diffuseAttenuate;
varying vec3 diffuseLightDirection;

uniform float eyeMultiplier;
uniform float skinMultiplier;
uniform float baseMultiplier;
uniform float fadeValue;

const float epsilon = 1e-6;

float calculateLight(){
	vec3 N = normal;
	vec3 L = diffuseLightDirection;
	
	float lambertTerm = dot(N,L) * diffuseAttenuate;
	return lambertTerm;
}

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

	float attenuate = (baseMultiplier + max(isEye() * eyeMultiplier, isSkin() * skinMultiplier)) * fadeValue;
	if(attenuate < epsilon){
		discard;
		return;
	}
	
    vec4 col = texture2DRect(rgbdTexture, gl_TexCoord[0].st);
    gl_FragColor = gl_Color * col * attenuate * max( calculateLight(), isSkin() );
	//gl_FragColor = vec4(normal,1.0);
}

