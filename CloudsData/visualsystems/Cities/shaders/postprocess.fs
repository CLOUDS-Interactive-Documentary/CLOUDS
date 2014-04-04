uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform vec2 resolution;
uniform vec2 textureResolution;
uniform float chromaDist;
uniform float grainDist;


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
	float offsetX = 0.0;
 	float offsetY = 0.0;
 	float zoom = 1.0;

	vec2 st = gl_FragCoord.xy/resolution.xy;// * vec2(1,-1);
	vec2 uv = (st*zoom);
	if (grainDist>0.0){
		float disp = texture2DRect(tex1, st*textureResolution ).r;
		uv.x += disp*grainDist*0.1;
	}

	vec2 offset = vec2(offsetX,offsetY);
	vec3 sumcol = vec3(0.0);
	vec3 sumw = vec3(0.0);	

	if ( chromaDist != 0.0 ){
		for ( int i=0; i<num_iter;++i ){
			float t = float(i) * reci_num_iter_f;
			vec3 w = spectrum_offset( t );
			sumw += w;
			sumcol += w * texture2DRect( tex0, (offset+barrelDistortion(uv, chromaDist*t))*resolution.xy ).rgb;
		}
		gl_FragColor = vec4(sumcol.rgb / sumw, 1.0);
	} else {
		gl_FragColor = texture2DRect( tex0, uv*resolution.xy );
	}

	
}
