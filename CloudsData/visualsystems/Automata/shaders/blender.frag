#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex;
uniform vec2 dims;
uniform vec4 frontColor1;
uniform vec4 frontColor2;

void main(void) 
{
    vec2 st = gl_TexCoord[0].st;
    float dist = length(st) / length(dims);
    vec4 frontColor = mix(frontColor1, frontColor2, dist);

    float amt = 1.0 - texture2DRect(tex, st).a;
    gl_FragColor = mix(frontColor, vec4(0.0, 0.0, 0.0, 0.0), amt);
}
