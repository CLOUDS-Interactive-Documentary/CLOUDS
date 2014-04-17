//#version 120
//#extension GL_ARB_texture_rectangle : enable

uniform float shininess;
uniform float dim;
uniform float facingRatio;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec4 vPos;
varying vec2 uv;

varying float fogMix;

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = abs(dot(ePos, normal));
	float spec = pow(fr, shininess);
	
	fr  = mix(1., fr * 1.3, facingRatio);

	gl_FragColor = vec4( color.xyz * fr + spec, 1.);
	
//	float atten = zDist / dim;
//	gl_FragColor.xyz *= min(1., 1. - atten * atten * atten);
	
	gl_FragColor.xyz *= fogMix;
}

