//these are our texture names, set in openFrameworks on the shader object in set up
uniform sampler2DRect image;
void main (void)
{
	//use the color from the image, but use the r channel of the mask as the alpha channel of our output
	gl_FragData[0] = texture2DRect(image, gl_TexCoord[0].st);
	
}
