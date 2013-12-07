
#version 120
#extension GL_ARB_texture_rectangle : enable

varying vec3 norm;
varying vec3 ePos;

float PI = 3.14159265359;
float HALF_PI = 1.57079632679;

void main()
{
	
	float sclr = 10.;
	
	float x = gl_Vertex.x * PI * sclr;
	float z = gl_Vertex.z * PI * sclr;
	float x1 = (gl_Vertex.x+.03) * PI * sclr;
	float z1 = (gl_Vertex.z+.03) * PI * sclr;

	vec3 p0 = vec3( gl_Vertex.x, sin(x) * cos(z) * .05, gl_Vertex.z );
	vec3 p1 = vec3( gl_Vertex.x, sin(x1) * cos(z) * .05, gl_Vertex.z );
	vec3 p2 = vec3( gl_Vertex.x, sin(x) * cos(z1) * .05, gl_Vertex.z );
	
	norm = normalize( gl_NormalMatrix * p0 );
	
	vec4 ecPosition = gl_ModelViewMatrix * vec4(p0, 1.);
	norm = norm * .5 + .5;
	
	
	
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

