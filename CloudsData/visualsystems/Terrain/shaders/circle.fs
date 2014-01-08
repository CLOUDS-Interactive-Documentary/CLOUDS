
uniform vec2 pos;
const float epsilon = 1e-6;

void main()
{
	float mag = length(pos);
	gl_FragColor.rgb = vec3(1.,1.,1.);
	gl_FragColor.a = pow(max(0.,1.0-mag),2.0);
}
