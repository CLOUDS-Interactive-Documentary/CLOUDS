#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;
uniform vec2 textureSize;
varying vec3 normal;

varying float VZPositionValid0;
const float epsilon = 1e-6;

void main(){
    
    if(VZPositionValid0 < epsilon){
    	discard;
        return;
    }

    vec4 col = texture2DRect(texture, gl_TexCoord[0].st);
    gl_FragColor = gl_Color * col;
}