
uniform float distanceMin;
uniform float distanceMax;

varying float normalizedDist;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin, outputMax);
}

void main(void)
{
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * pos;
	
	normalizedDist = map(-pos.z,distanceMin,distanceMax,0.,1.);
	
	gl_FrontColor = gl_Color;
}
