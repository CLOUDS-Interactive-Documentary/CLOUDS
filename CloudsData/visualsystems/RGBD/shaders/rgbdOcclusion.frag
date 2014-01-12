#version 110
#extension GL_ARB_texture_rectangle : enable

varying float positionValid;
const float epsilon = 1e-6;

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

	gl_FragColor.rgb = vec3(0.0);
	gl_FragColor.a = 1.0;
}

