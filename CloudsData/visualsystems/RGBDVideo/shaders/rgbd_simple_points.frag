#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;
varying float positionValid;
varying vec2 texSample;
const float epsilon = 1e-6;
uniform float colorFade;
uniform float alpha;

void main(){
	
    if(positionValid < epsilon){
		discard;
	}
    
    vec4 col = texture2DRect(texture, texSample);
    
    gl_FragColor.rgb = mix(col,gl_Color,colorFade).rgb * alpha;
    
    gl_FragColor.a = 1.0;
}

