#version 120
#extension GL_ARB_texture_rectangle : enable

varying float fogAmount;

void main()
{
    gl_FragColor = (1.0 - fogAmount) * gl_Color;
}