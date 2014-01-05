
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2D tex;
uniform vec4 spriteBaseColor;
uniform vec4 spritePopColor;
varying float activityValue;

void main(void){

    vec4 mixed = mix( spriteBaseColor,spritePopColor, activityValue)  ;
    vec4 color = texture2D(tex, gl_TexCoord[0].st);
//    vec4 mixed2 = mix(color,mixed, 0.3 );
	gl_FragColor = mixed * color ;
//    gl_FragColor = mixed2 ;
}
