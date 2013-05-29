#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect depthTex;
uniform sampler2DRect normalTex;

uniform float terrainHeight;

void main(void){
    vec2 st = gl_TexCoord[0].st;
    vec4 color = texture2DRect( depthTex, st );

    // gl_FragColor = color;

    gl_FragColor = gl_Color;
}
