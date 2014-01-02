
uniform vec4 lineColorNode;
uniform vec4 lineColorEdge;

uniform float colorMixExponent;
varying float colorMix;
varying float handleHide;
varying float distanceAttenuate;

void main (void) {
	//exponent and spread the color across
	vec4 mixColor = mix(lineColorNode,lineColorEdge,1.-pow(1.-colorMix,colorMixExponent));
	gl_FragColor.rgb = mixColor.rgb * mixColor.a * handleHide * distanceAttenuate;
	gl_FragColor.a = 1.0;

//	gl_FragColor = lineColorNode;
//	gl_FragColor.rgb = vec3(colorMix);
}
