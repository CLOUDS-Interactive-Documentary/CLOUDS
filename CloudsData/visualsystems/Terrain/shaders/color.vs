varying vec4 ecPosition;
varying vec3 ecPosition3;
varying vec3 normal;
varying vec4 ambientGlobal;

uniform sampler2DRect map;
uniform vec3 cameraPos;
varying vec4 fsColor;
varying float camDelta;

uniform sampler2DRect normalMap;
uniform float heightScale;

float lengthSqr(vec3 x)
{
	return dot(x,x);
}

void main()
{
    gl_TexCoord[0]  = gl_MultiTexCoord0;
    fsColor = texture2DRect(map,gl_TexCoord[0].st);

	ecPosition		= gl_ModelViewMatrix * gl_Vertex;
	ecPosition3		= vec3( ecPosition ) / ecPosition.w;
    vec3 n = texture2DRect(normalMap,gl_TexCoord[0].st).rgb;
    n.x  = n.x * 2. - 1.;
    n.y  = n.y * 2. - 1.;
    n.z  = n.z * 2. - 1.;
	normal			= gl_NormalMatrix * n;
    vec4 pos = gl_Vertex;
    pos.y =  1.0 - fsColor.r * heightScale;
	gl_Position		= gl_ModelViewProjectionMatrix * pos;
	
    
    camDelta = lengthSqr(cameraPos - gl_Vertex.xyz);
    
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
}