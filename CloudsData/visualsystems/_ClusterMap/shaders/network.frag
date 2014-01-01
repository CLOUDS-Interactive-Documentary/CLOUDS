
uniform vec3 lineColor;
void main (void) {
	gl_FragColor.rgb = lineColor*.1;
	gl_FragColor.a = 1.0;
//	gl_FragColor = vec4(1.,0.,0.,1.0);
}
