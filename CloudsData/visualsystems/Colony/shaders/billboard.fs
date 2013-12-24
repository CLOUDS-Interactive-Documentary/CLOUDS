uniform sampler2D tex;

void main (void) {
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
    c.b = pow(c.b, 2.);
	gl_FragColor = c * 0.8;

//	gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color;
}
