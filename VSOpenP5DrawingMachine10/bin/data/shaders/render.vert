#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posTex;
uniform vec2 screen;

void main() 
{
    // Use the texture coord as an index into the position texture.
    vec2 verPos = gl_MultiTexCoord0.xy;

    // Read the position data from texture.
    vec4 pixPos = texture2DRect(posTex, verPos);
    
    // Map the position from the texture (0.0 to 1.0) to the screen (0 - width/height)
    pixPos.x *= screen.x;
    pixPos.y *= screen.y;
    
    gl_Position = gl_ModelViewProjectionMatrix * pixPos;
    gl_FrontColor = gl_Color;
}
