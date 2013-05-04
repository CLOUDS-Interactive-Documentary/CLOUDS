#version 110
#extension GL_ARB_texture_rectangle : enable

uniform vec2 dim;
uniform vec2 shift;
uniform vec2 scale;

uniform sampler2DRect depthTex;
uniform vec2 principalPoint;
uniform vec2 fov;
uniform float farClip;
uniform float edgeClip;
uniform float xsimplify;
uniform float ysimplify;
uniform int useTexture;
uniform mat4 tTex;

varying float VZPositionValid0;

const float epsilon = 1e-6;

void main(void)
{
    //align to texture
    vec2 halfvec = vec2(.5,.5);
    float depth = texture2DRect(depthTex, floor(gl_Vertex.xy) + halfvec).r * 65535.;
    float right = texture2DRect(depthTex, floor(gl_Vertex.xy  + vec2(xsimplify,0.0)) + halfvec ).r * 65535.;
    float down  = texture2DRect(depthTex, floor(gl_Vertex.xy  + vec2(0.0,ysimplify)) + halfvec ).r * 65535.;
    float left  = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(-xsimplify,0.0)) + halfvec ).r * 65535.;
    float up    = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(0.0,-ysimplify)) + halfvec ).r * 65535.;
    float bl    = texture2DRect(depthTex, vec2(floor(gl_Vertex.x - xsimplify),floor( gl_Vertex.y + ysimplify)) + halfvec ).r * 65535.;
    float ur    = texture2DRect(depthTex, vec2(floor(gl_Vertex.x  + xsimplify),floor(gl_Vertex.y - ysimplify)) + halfvec ).r * 65535.;

    //TODO: make uniform var
    float nearClip = 20.0;
    //cull invalid verts
    VZPositionValid0 = (depth < farClip &&
                        right < farClip &&
                        down < farClip &&
                        left < farClip &&
                        up < farClip &&
                        bl < farClip &&
                        ur < farClip &&
                        
                        depth > nearClip &&
                        right > nearClip &&
                        down > nearClip &&
                        left > nearClip &&
                        up > nearClip &&
                        bl > nearClip &&
                        ur > nearClip &&
                        
                        abs(down - depth) < edgeClip &&
                        abs(right - depth) < edgeClip &&
                        abs(up - depth) < edgeClip &&
                        abs(left - depth) < edgeClip &&
                        abs(ur - depth) < edgeClip &&
                        abs(bl - depth) < edgeClip
                         ) ? 1.0 : 0.0;

    
	vec4 pos = vec4((gl_Vertex.x - principalPoint.x) * depth / fov.x,
                    (gl_Vertex.y - principalPoint.y) * depth / fov.y, depth, 1.0);

    //projective texture on the geometry
    if(useTexture == 1){
        
        vec4 texCd = tTex * pos;
        texCd.xyz /= texCd.w;
        
        texCd.y *= -1.;
        texCd.xy += 1.;
        texCd.xy /= 2.;
        
        texCd.xy *= scale;
        texCd.xy += shift;
        
        texCd.xy *= dim;
        gl_TexCoord[0] = texCd;
    }
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;

}
