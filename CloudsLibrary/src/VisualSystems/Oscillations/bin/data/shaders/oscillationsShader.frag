 #version 110
 // #extension GL_ARB_texture_rectangle : enable

 uniform sampler2DRect screen;
 uniform vec2 resolution;

void main(){

	gl_FragColor = texture2DRect(screen, gl_TexCoord[0].st);
	
}
