#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect rgbdTexture;
uniform float alpha;

//SKIN DETECTION SAMPLES
uniform vec3 skinSampleColor;
uniform vec3 skinWeights;
uniform vec2 skinThreshold;


varying float positionValid;

//LIGHTING
uniform vec3 actuatorDirection;
varying vec3 normal;
//varying vec3 eye;
//varying float diffuseAttenuate;
//varying vec3 diffuseLightDirection;
varying float headPositionAttenuation;
varying float edgeAttenuate;

const float epsilon = 1e-6;

float calculateLight(){
    return dot(normal,actuatorDirection);
}

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
    vec3 hslSample = rgb2hsl(skinSampleColor.rgb);
	vec3 hslCurrent = rgb2hsl(test0.rgb);
	
	//account for the fact that hue is an angular distance, not linear (ie .9 and .1 are actually just .2 apart)
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

	//TODO re-add attenuation
    vec4 col = texture2DRect(rgbdTexture, gl_TexCoord[0].st);
	float actuatorAttenuate = smoothstep(.3, .35, calculateLight()) ;
	gl_FragColor.rgb = col.rgb * mix(edgeAttenuate, 1.0, 0.0) * (1.0-headPositionAttenuation)*alpha * actuatorAttenuate;
	gl_FragColor.a = 1.0;
    
//    gl_FragColor.rgb = vec3(actuatorAttenuate);
//    gl_FragColor.rgb = normal;
	//apply light to just skin
//	gl_FragColor = gl_Color * col * attenuate * max( calculateLight(), isSkin() );
	//incorporating luminance from video, and skin
 //   gl_FragColor = gl_Color * col * attenuate * mix(calculateLight(), 1.0, isSkin() * min(pow(lum,2.0), 1.0) );
	//gl_FragColor = gl_Color * col * attenuate *  max(calculateLight(), isEye() );
	//gl_FragColor = vec4(normal,1.0);
}

