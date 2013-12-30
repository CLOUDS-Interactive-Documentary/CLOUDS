uniform sampler2DRect image;
uniform float extrusionAmount;

//RGB to brightness needs to be weighted based on our percpetion of value and color.
//Green informs most of what we see as the 'value' of a color, while blue very little
float sampleBrightness(vec3 color){
	vec3 grayWeight = vec3(0.3, 0.59, 0.11);
	return dot(grayWeight, color);
}

void main(void)
{
	//passes the texture coordinates along to the fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;

	//samples the texture at the vertex location to get a color
	float brightness = sampleBrightness( texture2DRect(image, gl_TexCoord[0].st).rgb );

	//create a new vertex that is the default vertex, plus move the Z along with the brightness
	vec4 extrudedVertex = vec4(gl_Vertex.x,
							   gl_Vertex.y,
							   gl_Vertex.z + brightness * extrusionAmount,
							   gl_Vertex.w);
	
	//position equation that is the same as ftransform() but takes into account our modified vertex
	gl_Position = gl_ModelViewProjectionMatrix * extrudedVertex;
}
