uniform float percentTraverseRevealed;
varying float freshTraversalPosition;
varying float finalizedTraversalBit;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp(((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin), outputMin,outputMax);
}

void main () {
	float traverseReveal = 1. - map(freshTraversalPosition, percentTraverseRevealed-.01, percentTraverseRevealed, 0.0, 1.0);
	
	gl_FragColor.rgb = gl_Color.rgb * traverseReveal;
	gl_FragColor.a = 1.0;
	
	gl_FragColor.rgb = vec3( max(traverseReveal, finalizedTraversalBit) );
}
