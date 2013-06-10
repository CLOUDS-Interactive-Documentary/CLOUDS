uniform sampler2DRect tex0;
uniform float textureScale;
uniform float scale;
uniform float pointsGap;
uniform float pointsAlpha;

float hex(vec2 p) 
{
	p.x *= 1.16;
	p.y += mod(floor(p.x), 4.0)*1.5;
	p = abs((mod(p, 1.0) - 0.5));
	return abs(max(p.x*1.5 + p.y, p.y*2.0) - 1.0);
}

void main() {
	vec2 position = gl_FragCoord.xy;

	float deep = 1.0 - texture2DRect(tex0,position/textureScale).r;

  	float stripe = (mod((position.x+position.y), scale*2.0) > scale / 1. ) ? 1.0 : 0.0;

	// vec2  p   = ( position )*scale*0.01;
	// float r   = .05;
	// float hexagram = smoothstep(0., r, hex(p));

	if (deep > 0.0){
		gl_FragColor = vec4(vec3(stripe),1.); //1.0 - hexagram, 
	} else {
		gl_FragColor = vec4( 0.0);
	}

	if (floor( mod(position.x, pointsGap)) == 0.0 &&  floor(mod(position.y, pointsGap) ) == 0.0) {
		gl_FragColor  = mix(gl_FragColor,vec4(1.0), pointsAlpha ); 
	}
}