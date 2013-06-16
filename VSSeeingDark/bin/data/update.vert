
//here are some example uniforms

void main(void)
{
	//this is a default vertex shader, don't modify the position
	gl_Position = ftransform();
	
	//.. and passes the  texture coordinates along to the fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	//pass color info along
	gl_FrontColor = gl_Color;
}
