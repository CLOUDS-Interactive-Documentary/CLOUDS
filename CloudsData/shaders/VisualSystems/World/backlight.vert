

varying vec3 eyeVec;

uniform float minAngle;
uniform float exponent;

//varying vec3 angleColored;
varying float angle;
varying vec3 vNorm;

void main(void)
{
	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	eyeVec = normalize(-vVertex);
	vNorm = normalize( gl_NormalMatrix * gl_Normal);
	
	angle = dot(vNorm, eyeVec);
		
	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
}
