#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect rgbdTexture;

varying float positionValid;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample; 

//LIGHTING
varying vec3 normal;

uniform float eyeMultiplier;
uniform float skinMultiplier;
uniform float baseMultiplier;
uniform float fadeValue;

uniform float headAttenuateMix;
varying float headPositionAttenuation;
varying float edgeAttenuate;

varying float actuatorExtendAttenuate;

uniform float meshAlpha;
uniform float colorBoost;
uniform float skinBoost;

//SKIN DETECTION SAMPLES
uniform vec3 skinSampleColor;
uniform vec3 skinWeights;
uniform vec2 skinThreshold;

varying vec4 normalcolor;
varying vec4 depthcolor;

const float epsilon = 1e-6;

////// END LIGHTS

////START SKIN STUFF
float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}
vec3 rgb2hsl( vec3 _input ){
	float h = 0.0;
	float s = 0.0;
	float l = 0.0;
	float r = _input.r;
	float g = _input.g;
	float b = _input.b;
	float cMin = min( r, min( g, b ) );
	float cMax = max( r, max( g, b ) );
	
	l = ( cMax + cMin ) / 2.0;
	if ( cMax > cMin ) {
		float cDelta = cMax - cMin;
        
		s = l < .05 ? cDelta / ( cMax + cMin ) : cDelta / ( 2.0 - ( cMax + cMin ) );
        
		// hue
		if ( r == cMax ) {
			h = ( g - b ) / cDelta;
		} else if ( g == cMax ) {
			h = 2.0 + ( b - r ) / cDelta;
		} else {
			h = 4.0 + ( r - g ) / cDelta;
		}
		
		if ( h < 0.0) {
			h += 6.0;
		}
		h = h / 6.0;
	}
	return vec3( h, s, l );
}

float weightedDistance(vec3 pnt1,vec3 pnt2,vec3 weights){
	vec3 v = pnt1 - pnt2;
	return sqrt(weights.x*(v.x*v.x) + weights.y*(v.y*v.y) + weights.z*(v.z*v.z) ) ;
}

float isSkin(){
    vec4 test0 = texture2DRect(rgbdTexture, gl_TexCoord[0].st);
    vec4 test1 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(1.0,0.0));
    vec4 test2 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(-1.0,0.0));
    vec4 test3 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(0.0,1.0));
    vec4 test4 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(0.0,-1.0));
    
    vec4 test5 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(1.0,1.0));
    vec4 test6 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(1.0,-1.0));
    vec4 test7 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(-1.0,1.0));
    vec4 test8 = texture2DRect(rgbdTexture, gl_TexCoord[0].st + vec2(-1.0,-1.0));
	
    vec4 avg = (test0+test1+test2+test3+test4+test5+test6+test7+test8)/9.;
    vec3 hslSample = rgb2hsl(skinSampleColor.rgb);
	
	//      vec3 hslCurrent = rgb2hsl(test.rgb);
    vec3 hslCurrent = rgb2hsl(avg.rgb);
	
	//account for the fact that hue is an angular distance, not linear (ie .9 and .1 are actually just .2 appart)
	float hueDist = hslCurrent.r - hslSample.r;
	if(hueDist > .5){
		hslCurrent.r -= 1.0;
	}
	else if(hueDist < -.5){
		hslCurrent.r += 1.0;
	}
	
    float dist = weightedDistance(hslSample.rgb,hslCurrent.rgb, normalize(skinWeights));
	float alpha = clamp(map(dist,skinThreshold.x,skinThreshold.y,1.0,0.0),0.,1.);
	return alpha;
}
////END SKIN STUFF

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

	float attenuate = 1.0;
    vec4 col = texture2DRect(rgbdTexture, gl_TexCoord[0].st);
    //vec4 col = normalcolor;
    //vec4 col = depthcolor;
	//gl_FragColor = col * mix( colorBoost + headPositionAttenuation, 1. + skinBoost, isSkin())  * edgeAttenuate * meshAlpha * smoothstep(0.2, .3, actuatorExtendAttenuate);
	gl_FragColor.rgb = col.rgb *
                       meshAlpha *
                       headPositionAttenuation *
                       smoothstep(.2, .3, actuatorExtendAttenuate) * // filter out the small points
                       mix(headPositionAttenuation, 1.0, isSkin()) * // 
                       edgeAttenuate;
    
//    gl_FragColor.rgb = vec3(1.0);
    
    gl_FragColor.a = 1.0;
}
