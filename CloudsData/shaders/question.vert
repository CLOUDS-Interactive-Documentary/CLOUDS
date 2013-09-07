uniform float expandPercent;
uniform float selectPercent;
uniform float maxExpand;

varying float selected;

uniform vec4 color;
const float epsilon = 1e-6;

void main(void)
{
	vec4 pos = gl_Vertex;
	pos.xyz += gl_Normal * expandPercent * maxExpand;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	//gl_Color.r is set to 1.0 if it's an outer ring
	//gl_Color.g contains the percent along the curve
	selected = max( gl_Color.r, gl_Color.g / max(selectPercent,epsilon) );

	gl_FrontColor = color;
}
