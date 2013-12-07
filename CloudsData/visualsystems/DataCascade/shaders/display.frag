
//varying float positionValid;
const float epsilon = 1e-6;
uniform sampler2DRect rgbdTexture;

uniform float minDist;
uniform float fallOff;
varying float fogFactor;
varying float wrapped;

void main (void)
{
	if(wrapped < 0.5){
		discard;
	}
	gl_FragColor = vec4(gl_Color.xyz,fogFactor);
	
}
