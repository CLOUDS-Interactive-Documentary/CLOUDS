#version 110
#extension GL_ARB_texture_rectangle : enable

vec4 lineNodeBase;
vec4 lineEdgeBase;

vec4 lineNodePop;
vec4 lineEdgePop;

varying float baseColorMix;
varying float popMix;

void main(){
    gl_FragColor = mix(mix(lineNodeBase, lineEdgeBase, edgePositionMix),
					   mix(lineNodePop, lineEdgePop, edgePositionMix),
					   popMix);
}

