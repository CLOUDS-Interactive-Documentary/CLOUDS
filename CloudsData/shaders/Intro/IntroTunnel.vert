
uniform sampler2DRect image;
uniform float minPointSize;
uniform float maxPointSize;
uniform float minDistance;
uniform float maxDistance;

void main(void)
{
	// passes the  texture coordinates along to the fragment shader
	//gl_TexCoord[0] = gl_MultiTexCoord0;

//	float offset = texture2DRect(image, vec2(gl_Vertex.x, 1.0)).r;
	//this is a default vertex shader, don't modify the position
//	pos.x += (texture2DRect(shift, vec2(gl_Vertex.x, pos.y)).r - .5) * deviation;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_PointSize = minPointSize;
	
	//pass color info along
	gl_FrontColor = gl_Color;
}
