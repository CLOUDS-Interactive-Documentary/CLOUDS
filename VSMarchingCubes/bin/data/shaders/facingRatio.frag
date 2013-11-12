varying vec3 norm;
varying vec3 ePos;

void main(void)
{
	float fr = dot( -ePos, norm) * .5 + .5;
	gl_FragColor = vec4( vec3(fr), 1.);
}

