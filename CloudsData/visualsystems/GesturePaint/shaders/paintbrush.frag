
uniform vec2 brushDimensions;
uniform vec2 paletteDimensions;
uniform float paletteExpandPercent;
uniform sampler2DRect palette;
uniform sampler2DRect brush;
uniform vec2 palettePosition;
uniform int useColorPalette;
const float epsilon = 1e-6;

varying vec2 v_texcoord;

void main()
{	
	vec4 brushSample = texture2DRect(brush, v_texcoord*brushDimensions);
	if(useColorPalette == 1){
		vec4 paletteSample = texture2DRect(palette, palettePosition + v_texcoord*paletteExpandPercent*paletteDimensions);
		gl_FragColor.rgb = paletteSample.rgb;
		gl_FragColor.a = brushSample.a;
	}
	else{
		gl_FragColor = brushSample * gl_Color;
	}
}
