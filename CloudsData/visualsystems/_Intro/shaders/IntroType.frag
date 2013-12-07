uniform float colorAttenuate;
uniform vec4 tint;
uniform sampler2D sprite;
uniform float useSprite;

void main (void)
{
		
	//vec4 spriteMix = mix( vec4(1.0), texture2D(sprite, gl_TexCoord[0].st), useSprite);
	//gl_FragColor = spriteMix * gl_Color * tint * colorAttenuate;
//	if(hover == 1){
//		gl_FragColor = vec4(1.0,0.0,0.0,1.0);
//	}
//	else{
	gl_FragColor = gl_Color;
//	}
}
