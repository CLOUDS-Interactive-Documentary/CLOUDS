uniform float focalPlane;
uniform float focalRange;
uniform float lineFade;

//uniform float radius;
uniform vec3 attractor;


void main() {
	
	float radius = 15.0;
	float flashRadius = 200.0;
	vec3 toPoint = gl_Vertex.xyz - attractor;
	float lengthWorld = sqrt(toPoint.x*toPoint.x + toPoint.y*toPoint.y + toPoint.z*toPoint.z);
	float attraction = pow(1.0 - min(lengthWorld, radius)/radius, 2.0);
	float flash = pow(1.0 - min(lengthWorld, flashRadius)/flashRadius, 4.0);
	
	vec4 eyeCoord = gl_ModelViewMatrix * (gl_Vertex - vec4(attraction*toPoint, 0.0));
	gl_Position = gl_ProjectionMatrix * eyeCoord;

	float dist = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	
//	gl_FrontColor = gl_Color;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	float attenuation = 1.0 - min(abs(gl_Position.z - focalPlane)/focalRange, 1.0);
//	vec4 flashed = vec4(mix(gl_Color.rgb, vec3(gl_Color.a), flash), gl_Color.a);
	vec4 flashed = mix(gl_Color, vec4(gl_Color.a), flash);
	gl_FrontColor = flashed * lineFade * attenuation * gl_Color.a;


}





