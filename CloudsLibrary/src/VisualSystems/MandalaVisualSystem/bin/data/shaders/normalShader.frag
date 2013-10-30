varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = pow( abs(dot( ePos, normal )), 16.);
	gl_FragColor = vec4( normal * .5 + .5 + fr, 1.);
//	gl_FragColor = vec4( uv + fr, 1., 1.);
}

