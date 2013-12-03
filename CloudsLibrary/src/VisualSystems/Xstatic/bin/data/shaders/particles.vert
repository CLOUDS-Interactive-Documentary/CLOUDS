#version 120

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 vertPos = vec4(gl_Vertex.xyz, 1.0);
	vec4 eyeCoord = gl_ModelViewMatrix * vertPos;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	
	float dist = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float att = 600.0 / dist;
	gl_PointSize = 50.0;//gl_Vertex.w * att;
	
	gl_FrontColor = gl_Color;
}
