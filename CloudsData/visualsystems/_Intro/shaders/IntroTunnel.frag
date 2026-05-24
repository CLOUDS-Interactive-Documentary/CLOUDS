#version 120

uniform float colorAttenuate;
uniform vec4 tint;
uniform sampler2D sprite;
uniform float useSprite;

void main (void)
{
	vec4 spriteMix = (useSprite > 0.5) ? texture2D(sprite, gl_PointCoord) : vec4(1.0);
	gl_FragColor = spriteMix * gl_Color * tint * colorAttenuate;
}
