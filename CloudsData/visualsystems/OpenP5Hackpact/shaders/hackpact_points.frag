#version 120

uniform sampler2D sprite;

void main(void) {
	gl_FragColor = texture2D(sprite, gl_PointCoord) * gl_Color;
}
