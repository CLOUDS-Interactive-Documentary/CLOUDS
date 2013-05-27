
uniform sampler2DRect image;
uniform float minPointSize;
uniform float maxPointSize;
uniform float minDistance;
uniform float maxDistance;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main(void)
{
	// passes the  texture coordinates along to the fragment shader
	//gl_TexCoord[0] = gl_MultiTexCoord0;

//	float offset = texture2DRect(image, vec2(gl_Vertex.x, 1.0)).r;
	//this is a default vertex shader, don't modify the position
//	pos.x += (texture2DRect(shift, vec2(gl_Vertex.x, pos.y)).r - .5) * deviation;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	//START NOISE ---------------------------
	gl_PointSize = clamp(map(pow(gl_Position.z,1.5), maxDistance, minDistance, minPointSize, maxPointSize),
							minPointSize, maxPointSize);
//	gl_PointSize = maxPointSize;
	//pass color info along
	gl_FrontColor = gl_Color;
}
