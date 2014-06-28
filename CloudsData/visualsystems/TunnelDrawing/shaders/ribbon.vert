#version 120

//here are some example uniforms
void main(void)
{
	//this is a default vertex shader, don't modify the position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		
	//pass color info along
	gl_FrontColor = gl_Color;
}
