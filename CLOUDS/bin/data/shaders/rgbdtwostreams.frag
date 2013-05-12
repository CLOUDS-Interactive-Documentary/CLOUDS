#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect colorTex;
uniform int useTexture;
uniform vec2 dim;
varying float VZPositionValid0;
const float epsilon = 1e-6;

void main()
{
    if(VZPositionValid0 < epsilon){
    	discard;
        return;
    }

    if(useTexture == 1){
        vec4 col = texture2DRect(colorTex, gl_TexCoord[0].st);
		//col = vec4(gl_TexCoord[0].s / dim.x, gl_TexCoord[0].t / dim.y, 0.0, 1.0);
        gl_FragColor = col * gl_Color;
    }
    else{
        gl_FragColor = vec4(0);
    }
	
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(VZPositionValid0);
}