uniform vec4 c1;
uniform vec4 c2;

varying vec4 color;
varying vec2 uv;

void main(void)
{
	gl_FragColor = mix( c1, c2, uv.x);
}

