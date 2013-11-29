uniform sampler2DRect tex;
uniform float maxHeight;

void main(void) 
{
	vec2 st = gl_MultiTexCoord0.st;

	vec4 bumpColor = texture2DRect(tex, st);
	// float df = 0.30 * bumpColor.r + 0.59 * bumpColor.g + 0.11 * bumpColor.b;
	// vec4 newVertexPos = vec4(gl_Normal * bumpColor.a * maxHeight, 0.0) + gl_Vertex;
	vec4 newVertexPos = vec4(gl_Normal * maxHeight * bumpColor.a, 0.0) + gl_Vertex;
	// vec4 newVertexPos = gl_Vertex;
	// newVertexPos.z += 100.0;

	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
	// gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   	gl_TexCoord[0] = gl_MultiTexCoord0; 
} 
