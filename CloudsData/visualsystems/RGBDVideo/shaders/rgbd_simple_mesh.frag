#version 110
#extension GL_ARB_texture_rectangle : enable

varying float positionValid;
const float epsilon = 1e-6;
uniform float colorFade;
uniform sampler2DRect texture;
varying vec2 texSample;

void main(){
	
    if(positionValid < epsilon){
		discard;
	}
	
    vec4 col = texture2DRect(texture, texSample);
	
//  gl_FragColor.rgb = mix(col,gl_Color,colorFade).rgb;
//	gl_FragColor.a = 1.0;
	
    gl_FragColor = col * gl_Color;
}

