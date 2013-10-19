uniform float expandPercent;
uniform float selectPercent;
uniform float maxExpand;

uniform float attenuateFade;
uniform float minDistance;
uniform float maxDistance;
varying float selected;

uniform vec4 color;
uniform vec4 selectedColor;

const float epsilon = 1e-6;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main(void)
{
	vec4 pos = gl_Vertex;
	pos.xyz += gl_Normal * expandPercent * maxExpand;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	//gl_Color.r is set to 1.0 if it's an outer ring
	//gl_Color.g contains the percent along the curve
	selected = max( gl_Color.r, gl_Color.g / max(selectPercent,epsilon) );
	
//	gl_FrontColor = color;
	float distanceAttenuate = clamp(map(gl_Position.z, minDistance, maxDistance, 1.0, 0.0), 0.0, 1.0);
	
	gl_FrontColor = mix(color,selectedColor,selectPercent) *  mix(1.0, distanceAttenuate, attenuateFade);
//	gl_FrontColor = color *  mix(1.0, distanceAttenuate, attenuateFade);
}
