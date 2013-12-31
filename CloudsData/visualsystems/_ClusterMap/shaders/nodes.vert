uniform float minSize;
uniform vec3 attractor;



void main() {
		//gl_PointSize = minSize * (gl_Normal.x+1.);// * att * nearAttenuate;
	gl_PointSize = 7.0;
	gl_FrontColor = gl_Color;//*nearAttenuate;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
/*
	float radius = 15.0;
	vec3 expandedVert = gl_Vertex.xyz;
	vec3 toPoint = expandedVert - attractor;
	float lengthWorld = sqrt(toPoint.x*toPoint.x + toPoint.y*toPoint.y + toPoint.z*toPoint.z);
	float attraction = pow(1.0 - min(lengthWorld, radius)/radius, 2.0);

	//vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	vec4 eyeCoord = gl_ModelViewMatrix * (vec4(expandedVert,1.0) - vec4(attraction*toPoint, 0.0));
	
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	float dist  = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float att	= 600. / dist;
	float nearMax = 25.0;
	float nearAttenuate = min(dist+10., nearMax)/nearMax;
	//gl_PointSize = minSize * (gl_Normal.x+1.);// * att * nearAttenuate;
	gl_PointSize = 4.0;
	gl_FrontColor = gl_Color;//*nearAttenuate;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	//gl_PointSize = 100.0;
//	gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
*/

}

