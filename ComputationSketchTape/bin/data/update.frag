
uniform sampler2DRect image;
uniform sampler2DRect speed;
uniform float speedMultiplier;
void main (void)
{
	//sample the current position
	float position = texture2DRect(image, gl_TexCoord[0].st ).r;
	
	//get the speed from the random offset texture
	float offset = texture2DRect(speed, gl_TexCoord[0].st ).r * speedMultiplier;

	//write the new position into the fp
	gl_FragColor = vec4(position + offset);
	
}
