

uniform sampler2DRect tex0;
uniform sampler2DRect distortionMap;
uniform vec2 resolution;
uniform vec2 dMapResolution;
uniform float chromaDist;
uniform float grainDist;
uniform float doBloom;
uniform float bloomAmount;
uniform int bloomSize;


vec2 barrelDistortion(vec2 coord, float amt) {
	vec2 cc = coord - 0.5;
	float dist = dot(cc, cc);
	return coord + cc * dist * amt;
}

float sat( float t ){
	return clamp( t, 0.0, 1.0 );
}

float linterp( float t ) {
	return sat( 1.0 - abs( 2.0*t - 1.0 ) );
}

float remap( float t, float a, float b ) {
	return sat( (t - a) / (b - a) );
}

vec3 spectrum_offset( float t ) {
	vec3 ret;
	float lo = step(t,0.5);
	float hi = 1.0-lo;
	float w = linterp( remap( t, 1.0/6.0, 5.0/6.0 ) );
	ret = vec3(lo,1.0,hi) * vec3(1.0-w, w, 1.0-w);
    
	return pow( ret, vec3(1.0/2.2) );
}

const int num_iter = 12;
const float reci_num_iter_f = 1.0 / float(num_iter);

void main(){
	float offsetX = 1.;
 	float offsetY = 1.;
 	float zoom = 2.0;
        
	vec2 uv = gl_TexCoord[0].st;
    
	vec2 offset = vec2(offsetX,offsetY);
    
    float mag = length(gl_TexCoord[0].st/resolution.xy - vec2(.5));
    float power = pow(max(0.,1.-mag), 2.6);
    
    
    if (grainDist>0.0){
		float disp = texture2DRect(distortionMap, (gl_TexCoord[0].st/resolution.xy)*dMapResolution ).r;
		uv.x += disp*grainDist*(dMapResolution.x*.05)*pow(max(0.,mag), 2.);
	}
    
	vec3 sumcol = vec3(0.0);
	vec3 sumw = vec3(0.0);
    vec4 sum = vec4(0);
    if(doBloom == 1.){
    for(int i= -bloomSize ;i < bloomSize; i++)
    {
        for (int j = -bloomSize; j < bloomSize; j++)
        {
            float m = pow(max(0.,1.-length(vec2(float(i)/float(bloomSize), float(j)/float(bloomSize)) - vec2(0.))),2.);
            sum += texture2DRect(tex0, gl_TexCoord[0].st + vec2(float(j), float(i)) )*m;
        }
    }
    }
    
	for ( int i=0; i<num_iter;++i ){
		float t = float(i) * reci_num_iter_f;
		vec3 w = spectrum_offset( t );
		sumw += w;
		sumcol += (w) * texture2DRect( tex0, (offset*barrelDistortion(uv/resolution.xy, chromaDist*t*power))*resolution.xy ).rgb;
	}
    
    vec4 color = vec4((sumcol.rgb + mix(vec3(0.),vec3(sum),bloomAmount) )/ sumw, 1.0);
    
	gl_FragColor =  color;
}
