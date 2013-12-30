varying vec3 norm;
varying vec3 ePos;

void main()
{
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	norm = gl_NormalMatrix * normalize(gl_Normal);
	//
	//	gl_TexCoord[0] = gl_MultiTexCoord0;
	//	gl_FrontColor = gl_Color;
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

