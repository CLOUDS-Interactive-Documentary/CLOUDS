varying vec3 col;
varying vec2 uv;

void main()
{

	uv = gl_MultiTexCoord0.xy;
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	
	col = gl_Color.xyz;
	
}