#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;

varying float positionValid;
const float epsilon = 1e-6;
uniform float colorFade;

varying vec2 texSample;

void main(){
	
    if(positionValid < epsilon){
		discard;
	}
	
    vec4 col = texture2DRect(texture, texSample);
	
	
    gl_FragColor = col * gl_Color;


}

