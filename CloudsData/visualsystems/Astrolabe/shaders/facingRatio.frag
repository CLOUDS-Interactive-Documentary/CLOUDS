#version 120

uniform float frScale = 1.;
uniform float frExpo = 32.;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;


void main(void)
{
	vec3 normal = normalize(norm);
	float fr = abs(dot( ePos, normal ));

	gl_FragColor = color * vec4(fr,fr,fr,1.);// + vec4( vec3( pow(fr*frScale, frExpo) ), 0.);
//	gl_FragColor.xyz *= fr;
//	gl_FragColor.xyz += color * pow(fr, 12.);
}

