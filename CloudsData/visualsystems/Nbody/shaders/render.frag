#version 110
#extension GL_ARB_texture_rectangle : enable

//uniform float spriteSize;

varying vec2 texcoord;

void main()
{
    gl_FragColor =  gl_Color;
}