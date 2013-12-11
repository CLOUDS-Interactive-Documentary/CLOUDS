varying vec4 color;

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	color = gl_Color;
}

