#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex;
uniform float fade;

void main(void) 
{
    vec2 st = gl_TexCoord[0].st;
    int sum = 0;

    vec4 currColor = texture2DRect(tex, st);
    vec4 liveColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 deadColor = vec4(0.0, 0.0, 0.0, fade);
    
    if (texture2DRect(tex, st + vec2(-1.0, -1.0)) == liveColor) ++sum;
    if (texture2DRect(tex, st + vec2( 0.0, -1.0)) == liveColor) ++sum;
    if (texture2DRect(tex, st + vec2( 1.0, -1.0)) == liveColor) ++sum;
 
    if (texture2DRect(tex, st + vec2(-1.0,  0.0)) == liveColor) ++sum;
    if (texture2DRect(tex, st + vec2( 1.0,  0.0)) == liveColor) ++sum;
 
    if (texture2DRect(tex, st + vec2(-1.0,  1.0)) == liveColor) ++sum;
    if (texture2DRect(tex, st + vec2( 0.0,  1.0)) == liveColor) ++sum;
    if (texture2DRect(tex, st + vec2( 1.0,  1.0)) == liveColor) ++sum;

    vec4 outColor;
    if (sum < 2 || sum > 3) outColor = deadColor;  // dead
    else if (sum == 3) outColor = liveColor;       // live
    else outColor = currColor;                     // same
    
    if (outColor != liveColor) {
        outColor.a *= fade;
    }
        
    gl_FragColor = outColor;
 }
