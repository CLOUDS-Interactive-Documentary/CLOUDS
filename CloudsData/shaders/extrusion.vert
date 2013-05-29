#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect depthTex;
uniform sampler2DRect normalTex;
uniform vec2  resolution;
uniform float terrainHeight;

///MAIN ---------------------------
void main(void)
{
    //align to texture
    vec2 st = gl_Vertex.xy;
    vec2 halfvec = vec2(.5,.5);

    float depth = texture2DRect(depthTex, floor(st) + halfvec).r;
    vec3 normal = texture2DRect(normalTex, floor(st) + halfvec).rgb;
    normal -= 0.5;
    normal *= 2.0;

	vec4 pos = vec4(st.x-resolution.x*0.5, st.y-resolution.y*0.5, depth*terrainHeight, 1.0);

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;
}
