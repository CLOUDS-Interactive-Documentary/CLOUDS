uniform sampler2DRect imgSampler;
uniform vec3 samplePointColor;
uniform vec3 weights;
uniform float lowerThreshold;
uniform float upperThreshold;
uniform int redGreenDebug;

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

float rgbToGray(vec4 rgbVal){
    float f =  0.21*rgbVal.r + 0.71*rgbVal.g + 0.07*rgbVal.b;
    return f;
}

//these are cubic easings
float easeOut(float t,float b , float c, float d) {
	return c*((t=t/d-1.)*t*t + 1.) + b;
}

float easeInOut(float t,float b , float c, float d)  {
	if ((t/=d/2.) < 1.) return c/2.*t*t*t + b;
	return c/2.*((t-=2.)*t*t + 2.) + b;
}

float mapEase(float value, float inputMin, float  inputMax, float  outputMin,float  outputMax, bool clamp ){
    
	if(clamp){
		value = clamp(value, inputMin, inputMax);
	}
    
	float t = value - inputMin;
	float c = outputMax - outputMin;
	float d = inputMax - inputMin;
	float b = outputMin;
	float res= easeInOut(t,b,c,d);
    
	return res;
}

float map(float value, float inputMin, float  inputMax, float  outputMin,float  outputMax ){
     float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
    return outVal;
}

float weightedDistance(vec3 pnt1,vec3 pnt2,vec3 weights){
	vec3 v = pnt1 - pnt2;
	return sqrt(weights.x*(v.x*v.x) + weights.y*(v.y*v.y) + weights.z*(v.z*v.z) ) ;
}

void main (void)
{
    vec4 test = texture2DRect(imgSampler, gl_TexCoord[0].st);
    vec4 test1 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(1.0,0.0));
    vec4 test2 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(-1.0,0.0));
    vec4 test3 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(0.0,1.0));
    vec4 test4 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(0.0,-1.0));
    
    vec4 test5 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(1.0,1.0));
    vec4 test6 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(1.0,-1.0));
    vec4 test7 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(-1.0,1.0));
    vec4 test8 = texture2DRect(imgSampler, gl_TexCoord[0].st + vec2(-1.0,-1.0));

    vec4 avg = (test + test1 +test2 + test3 +test4+test5+test6+test7+test8)/8.;
    vec3 hslSample = rgb2hsl(samplePointColor.rgb);


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
	
	
    float dist = weightedDistance(hslSample.rgb,hslCurrent.rgb, normalize(weights));
	//float alpha = mapEase(dist,lowerThreshold,upperThreshold,0.,1.0,true);
	float alpha = clamp(map(dist,lowerThreshold,upperThreshold,1.0,0.0),0.,1.);
	if(redGreenDebug == 1){
		gl_FragColor = mix(vec4(alpha,1.0-alpha,0.0, 1.0), test, .5);
	}
	else{
		gl_FragColor = vec4(test.rgb,1.0)*alpha;
	}
}

