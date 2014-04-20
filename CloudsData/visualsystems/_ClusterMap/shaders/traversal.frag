uniform float trailVertCount;
uniform float segmentVertCount;
uniform float trailRevealCount;
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
    
    //this is how far along the reveal is on the front
	float traverseReveal = 1.0 - smoothstep(percentTraverseRevealed, percentTraverseRevealed+.01,freshTraversalPosition);
    
    //this calculates the fall off on the back of the trail
    float vertsRemainingReveal = segmentVertCount * (1.0 - percentTraverseRevealed);
    float revealedVertsInTrail = trailVertCount - vertsRemainingReveal;
    float trailFadeStart = revealedVertsInTrail;
    float trailFadeEnd = revealedVertsInTrail - trailRevealCount;
    float falloff = smoothstep(trailFadeEnd, //from the end of the trail
                               trailFadeStart, //to the head of the last segment
                               totalTraversalPosition);
    
    //mix the alpha
    float trailAttenuate = falloff * max(traverseReveal,finalizedTraversalBit);
    
	//float segmentVertexPosition = freshTraversalPosition * segmentVertCount;
//	float colorMix = map(segmentVertCount*traverseReveal - segmentVertexPosition, 0.0, colorFalloff*10., 0., 1.0);
    //blend the two primary colors together
    float colormix = smoothstep(colorFalloff, .995,trailAttenuate);
    //then flash the tip white
    float whiteflash = smoothstep(.995,1.0,trailAttenuate);
    //mix em together
//	vec4 result = mix( mix(headColor, tailColor, colormix), vec4(1.0), whiteflash);
	vec4 result = mix( mix(tailColor, headColor, colormix), vec4(1.0), whiteflash);
    
    gl_FragColor.rgb = result.rgb * trailAttenuate;
	gl_FragColor.a = 1.0;
//	gl_FragColor.rgb = vec3( falloff*max(traverseReveal,finalizedTraversalBit) );
}
