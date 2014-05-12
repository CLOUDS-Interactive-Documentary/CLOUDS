#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2D tex;

void main (void) {
	vec4 color = texture2D(tex, gl_TexCoord[0].st);
	gl_FragColor = color * gl_Color * color.a;
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}
