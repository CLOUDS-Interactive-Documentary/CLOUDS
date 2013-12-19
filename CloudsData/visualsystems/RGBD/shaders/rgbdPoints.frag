#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect rgbdTexture;

varying float positionValid;
//varying vec4 faceFeatureSample;
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

uniform float headAttenuateMix;
varying float headPositionAttenuation;
varying float edgeAttenuate;
varying float forceFade;

const float epsilon = 1e-6;

float calculateLight(){
	vec3 N = normal;
	vec3 L = diffuseLightDirection;
	
	float lambertTerm = dot(N,L) * diffuseAttenuate;
	return lambertTerm;
}

float isSkin(){
	return 0.0;
}

float videoBrightness(vec3 c){
	return 0.21*c.r + 0.71*c.g + 0.07*c.b;
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

//	float attenuate =  max(isEye() * eyeMultiplier, isSkin() * skinMultiplier)) * fadeValue * forceFade;
//	if(attenuate < epsilon){
//		discard;
//		return;
//	}
	
    vec4 col = texture2DRect(rgbdTexture, gl_TexCoord[0].st);
	float lum = videoBrightness(col.rgb);
	
	/////TEST WHITE
//	gl_FragColor = vec4(1.0);
	/////
	
	/////basic coloring
	gl_FragColor = gl_Color * col;
	//////
		
	//apply lighting universaly
    //gl_FragColor = gl_Color * col * attenuate * calculateLight();
	
	//apply lighting based on video luminosity
//	gl_FragColor = gl_Color * col * 
//					mix( mix(calculateLight(),1.0,isSkin()), 1.0, pow(lum,2.0) ) *
//					mix(1.0-headPositionAttenuation,headPositionAttenuation, headAttenuateMix) * edgeAttenuate;
	
	//apply light to just skin
//	gl_FragColor = gl_Color * col * attenuate * max( calculateLight(), isSkin() );
	
	//incorporating luminance from video, and skin
 //   gl_FragColor = gl_Color * col * attenuate * mix(calculateLight(), 1.0, isSkin() * min(pow(lum,2.0), 1.0) );
		
	//gl_FragColor = vec4(normal,1.0);
}

