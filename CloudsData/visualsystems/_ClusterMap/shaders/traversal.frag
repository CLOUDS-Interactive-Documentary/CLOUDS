uniform float trailVertCount;
uniform float segmentVertCount;
uniform vec4 headColor;
uniform vec4 tailColor;
uniform float colorFalloff;

uniform float percentTraverseRevealed;
varying float freshTraversalPosition;
varying float finalizedTraversalBit;
varying float totalTraversalPosition;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin,outputMax);
}

void main () {
//	float currentTraverseReveal = map(freshTraversalPosition, percentTraverseRevealed, 1.0, 0.0, 1.0);
	float traverseReveal = 1. - map(freshTraversalPosition, percentTraverseRevealed-.01, percentTraverseRevealed, 0.0, 1.0);

	float segmentVertexPosition = freshTraversalPosition * segmentVertCount;
	float colorMix = map(segmentVertCount*traverseReveal - segmentVertexPosition, 0.0, colorFalloff*10., 0., 1.0);
	vec4 result = mix(headColor, tailColor, max(colorMix,finalizedTraversalBit));
	gl_FragColor.rgb = result.rgb * result.a * max(traverseReveal,finalizedTraversalBit);
	gl_FragColor.a = 1.0;
//	gl_FragColor.rgb = vec3(colorMix);
//	gl_FragColor.rgb = vec3( ( (trailVertCount-(segmentVertCount-currentVert)) - totalTraversalPosition) / segementVertCount );
}
