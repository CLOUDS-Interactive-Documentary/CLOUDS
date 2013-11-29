varying vec2 texture_coordinate;
varying vec2 screenPosition;
void main(void)
{

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	
	gl_TexCoord[0] = gl_MultiTexCoord0;

}
