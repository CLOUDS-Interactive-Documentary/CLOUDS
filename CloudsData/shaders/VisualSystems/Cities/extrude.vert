#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect depthTex;
uniform vec2  resolution;
uniform float height;
uniform float alpha;
uniform float pointSize;

void main(void)
{
    //align to texture
    vec2 st = gl_Vertex.xy;
    vec2 halfvec = vec2(1.0);

    float depth = texture2DRect(depthTex, floor(st) + halfvec).r;    
	vec4 pos = vec4(st.x-resolution.x*0.5, st.y-resolution.y*0.5, depth*height, 1.0);
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_PointSize = pointSize;

    if (depth == 0.0){
        gl_FrontColor = vec4(0.0);
    } else {
        gl_FrontColor = gl_Color;
        gl_FrontColor.a = depth*alpha;
    }
}
