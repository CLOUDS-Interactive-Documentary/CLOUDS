uniform sampler2DRect tex0;
uniform float textureScale;
uniform float scale;

uniform float stripeAlpha;
uniform float hexAlpha;

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

  	float stripe = (mod((position.x+position.y), scale*2.0) > scale / (0.5+deep*0.5) ) ? 1.0 : 0.0;

	vec2  p   = ( position )*scale*0.09;
	float r   = deep*0.3;
	float hexagram = smoothstep(0., r, hex(p));

	if (deep > 0.01){
		gl_FragColor  = vec4(vec3(1.0 - hexagram), hexAlpha ); 
		gl_FragColor  = mix(gl_FragColor,vec4(stripe), stripeAlpha ); 
	} else {
		gl_FragColor  = vec4(vec3(1.0 - hexagram),deep*100.0*hexAlpha); 
		gl_FragColor  = mix(gl_FragColor,vec4(vec3(stripe),deep*100.0), stripeAlpha );
	}

	if (floor( mod(position.x, pointsGap)) == 0.0 &&  floor(mod(position.y, pointsGap) ) == 0.0) {
		gl_FragColor  = mix(gl_FragColor,vec4(1.0), pointsAlpha ); 
	}
}