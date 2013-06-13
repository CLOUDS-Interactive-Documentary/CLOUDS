#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;

varying float positionValid;
varying vec3 normal;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample;

const float epsilon = 1e-6;

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

    vec4 col = texture2DRect(texture, gl_TexCoord[0].st);
	
//    gl_FragColor = gl_Color * col + faceFeatureSample;
    gl_FragColor = gl_Color * col;	
}