#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex;
uniform vec4 frontColor;

void main(void) 
{
    float amt = 1.0 - texture2DRect(tex, gl_TexCoord[0].st).a;
    gl_FragColor = mix(frontColor, vec4(0.0, 0.0, 0.0, 0.0), amt);
}
