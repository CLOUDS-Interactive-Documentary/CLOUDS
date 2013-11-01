varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = abs(dot( ePos, normal ));

	gl_FragColor = color;
	gl_FragColor.xyz *= fr;
	gl_FragColor.xyz += pow(fr, 128.);
}

