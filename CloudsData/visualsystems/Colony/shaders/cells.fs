#version 120
// #extension GL_ARB_texture_rectangle : enable

void main(){
    vec4 c = gl_Color;
    c.a = pow(c.a, 6);
	gl_FragColor += c;
    
}
