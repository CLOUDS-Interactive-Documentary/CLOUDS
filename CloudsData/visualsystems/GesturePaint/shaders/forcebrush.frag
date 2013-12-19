
/* VBlurVertexShader.glsl */

//uniform float radius;
varying vec2 pos;

const float epsilon = 1e-6;

void main()
{
//	vec2 center = vec2(radius,radius);
//	vec2 direction = pos - center;
//	vec2 directionNormal = (direction / dimensions);
	float mag = length(pos);
	//gl_FragColor.rg = pos*.5+.5;
	gl_FragColor.rg = pos;
	gl_FragColor.b = 0.0;
	gl_FragColor.a = pow(max(0.,1.0-mag),2.0);
}
