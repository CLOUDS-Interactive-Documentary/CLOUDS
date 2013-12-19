
/* VBlurVertexShader.glsl */

uniform vec2 dimensions;
varying vec2 pos;

const float epsilon = 1e-6;

void main()
{
	vec2 center = dimensions * .5;
	vec2 direction = pos - center;
	vec2 directionNormal = (direction / dimensions);

	gl_FragColor.rg = directionNormal;
	gl_FragColor.b = 1.0;
	gl_FragColor.a = pow(1.0-length(directionNormal),2.0);
}
