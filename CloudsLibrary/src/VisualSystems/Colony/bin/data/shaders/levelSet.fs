#version 120
// #extension GL_ARB_texture_rectangle : enable
#define PI 3.14159265359

uniform sampler2DRect tex;

void main(){
    vec4 samp = texture2DRect(tex, gl_TexCoord[0].xy) * gl_Color;
    float levl = 0.5 * (1. + sin(-PI * 0.5 + length(samp.xyz) * 5. * PI));
	gl_FragColor = vec4(vec3(levl),1.);
    
}
