uniform float colorAttenuate;
void main (void)
{
	gl_FragColor = gl_Color * colorAttenuate;
}
