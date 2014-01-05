#version 110
#extension GL_ARB_texture_rectangle : enable

uniform vec4 lineNodeBase;
uniform vec4 lineEdgeBase;

uniform vec4 lineNodePop;
uniform vec4 lineEdgePop;

uniform float synapseLevel;
uniform vec4 synapse;

uniform float edgeInterpolateExponent;

varying float edgePositionMix;
varying float activityValue;
varying float synapseMix;

void main(){

  gl_FragColor = mix(mix(mix(lineNodeBase, lineEdgeBase, 1.0-pow(edgePositionMix, edgeInterpolateExponent)),
					     mix(lineNodePop,  lineEdgePop, 1.0-pow(edgePositionMix, edgeInterpolateExponent)),
					     activityValue), synapse, synapseMix*synapseLevel);

//    gl_FragColor = synapse;
//    if(activityValue > 0.){
//        gl_FragColor = mix(lineNodePop, lineEdgePop, 1.0-pow(edgePositionMix, edgeInterpolateExponent))* activityValue;
//    }
//    else{
//        gl_FragColor = mix(lineNodeBase, lineEdgeBase, 1.0-pow(edgePositionMix, edgeInterpolateExponent ) ) ;
//  
//    }
    
//    gl_FragColor = mix(lineNodeBase, lineEdgeBase, 1.0-pow(edgePositionMix, edgeInterpolateExponent))   * activityValue;
    //	gl_FragColor.rgb = vec3(activityValue);
}

