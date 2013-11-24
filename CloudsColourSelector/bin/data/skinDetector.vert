varying vec2 texture_coordinate;
varying vec2 screenPosition;
void main(void)
{
	//this is a default vertex shader, don't modify the position
    //this is outdated
    //	gl_Position = ftransform();
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	//.. and passes the  texture coordinates along to the fragment shader
    //gl_MultiTexCoord0
	gl_TexCoord[0] = gl_MultiTexCoord0;
	screenPosition	= gl_Vertex.xy;
//    texture_coordinate = vec2(gl_MultiTexCoord0);
}
