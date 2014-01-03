
uniform vec4 lineColorNode;
uniform vec4 lineColorEdge;
uniform float flickerIntensity;
uniform float colorMixExponent;
varying float colorMix;
varying float handleHide;
varying float distanceAttenuate;
varying float flickerSample1;
varying float flickerSample2;

void main (void) {
	//exponent and spread the color across
	vec4 mixColor = mix(lineColorNode,lineColorEdge,1.-pow(1.-colorMix,colorMixExponent));
	gl_FragColor.rgb = mixColor.rgb * mixColor.a * handleHide * max(distanceAttenuate, (flickerSample1 + flickerSample2) * flickerIntensity);
	gl_FragColor.a = 1.0;
	//gl_FragColor.rgb = vec3(flickerSample1+flickerSample2*20.);
	
//	gl_FragColor = lineColorNode;
//	gl_FragColor.rgb = vec3(colorMix);
}
