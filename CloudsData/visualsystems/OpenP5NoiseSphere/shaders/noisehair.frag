#version 110
#extension GL_ARB_texture_rectangle : enable

uniform float alphaDamp;
void main(void){
    gl_FragColor = gl_Color;
	gl_FragColor.a *= alphaDamp;
}
