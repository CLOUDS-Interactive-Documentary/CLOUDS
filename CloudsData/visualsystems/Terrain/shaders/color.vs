varying vec4 ecPosition;
varying vec3 ecPosition3;
varying vec3 normal;
varying vec4 ambientGlobal;

uniform sampler2DRect map;
uniform vec3 cameraPos;
varying vec4 fsColor;
varying float camDelta;

float lengthSqr(vec3 x)
{
	return dot(x,x);
}

void main()
{
    gl_TexCoord[0]  = gl_MultiTexCoord0;
    
	ecPosition		= gl_ModelViewMatrix * gl_Vertex;
	ecPosition3		= vec3( ecPosition ) / ecPosition.w;
    
	normal			= gl_NormalMatrix * gl_Normal;
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;
	
    fsColor = texture2DRect(map,gl_TexCoord[0].st);
    
    camDelta = lengthSqr(cameraPos - gl_Vertex.xyz);
    
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
}