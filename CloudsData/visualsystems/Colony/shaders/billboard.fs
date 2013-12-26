uniform sampler2D tex;
varying float size;

void main (void) {
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
    c.g += 0.3 * clamp(pow(c.g + 1., size * 0.05) - 1., 0., 1.);
	gl_FragColor = c * 0.8;

//	gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color;
}
