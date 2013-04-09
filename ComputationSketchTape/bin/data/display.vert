
uniform sampler2DRect image;
uniform sampler2DRect shift;
uniform float height;

void main(void)
{
	//.. and passes the  texture coordinates along to the fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;

	float offset = texture2DRect(image, vec2(gl_Vertex.x, 1.0)).r;
	
	//this is a default vertex shader, don't modify the position
	vec4 pos = vec4(gl_Vertex.x,
					mod(gl_Vertex.y + offset, height),
					gl_Vertex.z,
					gl_Vertex.w);

	pos.x += (texture2DRect(shift, vec2(gl_Vertex.x, pos.y)).r - .5) * 750.;
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
	
	//pass color info along
	gl_FrontColor = gl_Color;
}
