

varying float fogFactor;
const float LOG2 = 1.442695;
varying vec3 eyeVec;
varying vec3 normal;

void main(void)
{
	
	//----------
	//FOG STUFF
	vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;

	gl_FogFragCoord = length(vVertex);
	fogFactor = exp2(-gl_Fog.density *
					 gl_Fog.density  *
					 gl_FogFragCoord *
					 gl_FogFragCoord *
					 LOG2);
	
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	//END FOG

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	normal = -normalize(gl_NormalMatrix * gl_Normal);
	eyeVec = normalize(-vVertex);
	
	gl_FrontColor = gl_Color;
}

