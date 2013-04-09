
//here are some example uniforms
uniform sampler2DRect image;

void main (void)
{
	//sample the color
	//vec4 sample = texture2DRect(image, gl_TexCoord[0].st);
	
	gl_FragColor = gl_Color;
	
}
