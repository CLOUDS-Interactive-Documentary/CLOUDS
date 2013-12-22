#version 120
// #extension GL_ARB_texture_rectangle : enable

void main(){
    vec4 c = gl_Color;
    c.w = pow(c.w, 8);
	gl_FragColor = c;
    
}
