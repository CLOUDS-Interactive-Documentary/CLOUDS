#version 110
#extension GL_ARB_texture_rectangle : enable


//Shader for visualizing a depth texture as a focal distance map
//paired with dofblur for debugging purposes

uniform sampler2DRect depthTex;
uniform float focalDistance;
uniform float focalRange;

float LinearizeDepth(float zoverw){
    float n = 1.0; // camera z near
    float f = 20000.0; // camera z far
    return (2.0 * n) / (f + n - zoverw * (f - n));
}

void main()
{
    float depth = LinearizeDepth( texture2DRect(depthTex, gl_TexCoord[0].st).r ) * 20000.;
    gl_FragColor = vec4(min( abs(depth  - focalDistance) / focalRange, 1.0) );
    gl_FragColor.a = 1.0;
}