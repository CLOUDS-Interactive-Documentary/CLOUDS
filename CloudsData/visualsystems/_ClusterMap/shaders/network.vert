
uniform float focalPlane;
uniform float focalRange;
uniform vec3 attractor;

uniform sampler2DRect flickerTex;

varying float distanceAttenuate;
varying float colorMix;
varying float handleHide;

varying float flickerSample1;
varying float flickerSample2;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin,outputMax);
}

void main() {
	vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	distanceAttenuate = 1.0 - map(abs(gl_Position.z - focalPlane), 0.0, focalRange, 0.0, 1.0);

	//normal varies across the edges 0 -> 1,
	//so let's convert it to is 1, middle is 0
	colorMix = 1.-(abs(gl_Normal.x - .5) * 2.);
	handleHide = gl_Normal.z;
	
	flickerSample1 = texture2DRect(flickerTex, gl_Color.rg).r;
	flickerSample2 = texture2DRect(flickerTex, gl_Color.ba).r;
	/*
	float radius = 15.0;
	float flashRadius = 200.0;
	vec3 expandedPos = gl_Vertex.xyz;
	vec3 toPoint = expandedPos - attractor;
	float lengthWorld = sqrt(toPoint.x*toPoint.x + toPoint.y*toPoint.y + toPoint.z*toPoint.z);
	float attraction = pow(1.0 - min(lengthWorld, radius)/radius, 2.0);
	float flash = pow(1.0 - min(lengthWorld, flashRadius)/flashRadius, 4.0);
	
	vec4 eyeCoord = gl_ModelViewMatrix * (vec4(expandedPos,1.0) - vec4(attraction*toPoint, 0.0));
	gl_Position = gl_ProjectionMatrix * eyeCoord;

	float dist = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	
//	gl_FrontColor = gl_Color;
//	gl_TexCoord[0] = gl_MultiTexCoord0;
	float attenuation = 1.0 - min(abs(gl_Position.z - focalPlane)/focalRange, 1.0);
//	vec4 flashed = vec4(mix(gl_Color.rgb, vec3(gl_Color.a), flash), gl_Color.a);
	vec4 flashed = mix(color, vec4(gl_Color.a), flash);
	gl_FrontColor = mix(flashed * lineFade * attenuation * color.a, vec4(.5, .2, .2, .2), gl_Normal.x);
	 */
}





