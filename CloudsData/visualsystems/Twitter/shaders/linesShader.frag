#version 110
#extension GL_ARB_texture_rectangle : enable

uniform vec4 lineNodeBase;
uniform vec4 lineEdgeBase;

uniform vec4 lineNodePop;
uniform vec4 lineEdgePop;

uniform float edgeInterpolateExponent;

varying float edgePositionMix;
varying float popMix;
varying float activityValue;

void main(){
    gl_FragColor = mix(mix(lineNodeBase, lineEdgeBase, 1.0-pow(edgePositionMix, edgeInterpolateExponent)),
					   mix(lineNodePop, lineEdgePop, 1.0-pow(edgePositionMix, edgeInterpolateExponent)),
					   popMix) * activityValue;
//	gl_FragColor.rgb = vec3(activityValue);
}

