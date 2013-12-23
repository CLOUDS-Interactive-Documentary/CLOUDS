uniform sampler2D tex;

void main (void) {
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
    c.a = pow(c.a, 2.);
	gl_FragColor = c;

//	gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color;
}
