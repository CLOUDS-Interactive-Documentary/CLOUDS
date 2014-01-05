
uniform float pointSizeMin;
uniform float pointSizeMax;
uniform float pointDistanceMin;
uniform float pointDistanceMax;
uniform float blipFalloffExponent;

varying float normalizedDist;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin, outputMax);
}

void main(void)
{
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * pos;
	normalizedDist = map(-pos.z,pointDistanceMin,pointDistanceMax,0.,1.);
	//normalizedDist = -pos.z / pointDistanceMax;
	gl_PointSize = map(pow(1. - normalizedDist,4.), 0.0, 1.0, pointSizeMin, pointSizeMax*3.);
	gl_FrontColor = gl_Color;
}
