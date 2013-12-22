uniform float cutoff;
uniform float alphaSmoothExpo;
uniform float alphaSmoothScale;

uniform vec3 noduleColor;

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;
varying vec4 color;

float mapLinear( float x,float a1,float a2,float b1,float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

void main(void)
{
//	vec3 normal = normalize(norm);
//	float fr = pow( abs(dot( ePos, normal )), 32.);
	//	gl_FragColor = vec4( normal * .5 + .5 + fr, 1.);

	float alpha;

	if(uv.x > cutoff)
	{
		alpha = 1. - abs(mapLinear( uv.x, cutoff, 1., 0.,1. ) * 2. - 1.);
		
		alpha = pow( alpha * alphaSmoothScale, alphaSmoothExpo);
	}
	else
	{
		alpha = 0.;
	}

	gl_FragColor = color;
	gl_FragColor.w *= min(1., alpha);
}

