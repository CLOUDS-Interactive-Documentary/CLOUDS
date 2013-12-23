#version 120

uniform sampler2D tex;

void main(){
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
//    c.a = pow(c.a, 10);
	gl_FragColor += c;
    
}
