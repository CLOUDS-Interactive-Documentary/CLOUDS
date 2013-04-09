
//here are some example uniforms
uniform sampler2DRect image;
uniform sampler2DRect speed;

void main (void)
{
	//sample the color
	float sample = texture2DRect(image, gl_TexCoord[0].st ).r;
	float offset = texture2DRect(speed, gl_TexCoord[0].st ).r * .1;

	gl_FragColor = vec4(sample + offset);
	
}
