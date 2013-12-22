uniform bool bRenderNormals;
uniform vec4 fgColor;

varying vec3 norm;

void main(void)
{
	if (bRenderNormals) {
		gl_FragColor = vec4(norm, 1.0);	
	}
	else {
		gl_FragColor = vec4(fgColor);	
	}
}
