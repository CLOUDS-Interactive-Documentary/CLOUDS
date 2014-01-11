varying vec3 norm;
varying vec3 ePos;

varying vec4 ecPosition;
varying vec3 ecPosition3;
varying vec3 normal;
varying vec4 ambientGlobal;

varying float camDelta;

float lengthSqr(vec3 x)
{
	return dot(x,x);
}

void main(void)
{
	norm = normalize(gl_NormalMatrix * gl_Normal);
	norm = norm * .5 + .5;
    
    gl_TexCoord[0]  = gl_MultiTexCoord0;
	ecPosition		= gl_ModelViewMatrix * gl_Vertex;
	ecPosition3		= vec3( ecPosition ) / ecPosition.w;
	normal			= gl_NormalMatrix * gl_Normal;
    vec4 pos = gl_Vertex;
	gl_Position		= gl_ModelViewProjectionMatrix * pos;
	
    
    camDelta = lengthSqr(ecPosition3 - gl_Vertex.xyz);
    
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	//vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	//gl_Position = gl_ProjectionMatrix * ecPosition;
}
