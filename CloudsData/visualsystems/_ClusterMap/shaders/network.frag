
uniform vec4 lineColorNode;
uniform vec4 lineColorEdge;
uniform float colorMixExponent;
varying float colorMix;
varying float handleHide;

void main (void) {
	//exponent and spread the color across
	vec4 mixColor = mix(lineColorEdge,lineColorNode,pow(colorMix,colorMixExponent));
	gl_FragColor.rgb = mixColor.rgb * mixColor.a * handleHide;
	gl_FragColor.a = 1.0;

//	gl_FragColor = lineColorNode;
//	gl_FragColor.rgb = vec3(colorMix);
}
