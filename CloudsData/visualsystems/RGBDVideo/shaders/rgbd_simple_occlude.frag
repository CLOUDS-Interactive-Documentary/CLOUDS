#version 110
#extension GL_ARB_texture_rectangle : enable

varying float positionValid;
const float epsilon = 1e-6;
uniform float colorFade;

void main(){
	
    if(positionValid < epsilon){
		discard;
	}
    
    gl_FragColor = vec4(1.);
}

