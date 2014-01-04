
#version 120
#extension GL_ARB_texture_rectangle : enable

//vec4 nodeBaseColor;
//vec4 nodePopColor;
uniform sampler2D tex;




void main(void){

    vec4 color = texture2D(tex, gl_TexCoord[0].st);
	gl_FragColor = color * gl_Color * color.a ;
//	gl_FragColor = color * gl_Color * color.a * mixed;
//    gl_FragColor =  mixed;
    //	gl_FragColor = gl_Color;

}
