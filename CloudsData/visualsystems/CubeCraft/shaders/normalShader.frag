uniform float drawCenters;
uniform float noiseCutoff;
uniform float cameraCutoffDistance;
uniform float edgeThreshold;

varying vec3 vertex;

varying float nScl;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

varying float camDelta;

float mapLinear( float x, float a1, float a2, float b1, float b2 ) {
	return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

void main(void)
{
	if( nScl < noiseCutoff)	discard;
	if( camDelta < cameraCutoffDistance) discard;
	
	vec3 v = abs(vertex) * 2.;
	float edgeSample = max( v.x * v.y, max( v.y * v.z, v.z * v.x ));

	if(edgeSample > edgeThreshold)
	{
		gl_FragColor = vec4(1.);
	}
	else
	{
		if(int(drawCenters) == 0)	discard;
		
		vec3 normal = normalize(norm);
		float fr = pow( abs(dot( ePos, normal )), 2.);

		vec4 c1 = vec4(0.,0.,0., 1.);
		vec4 c2 = vec4(.2,.24,.5, 1.);
		
		gl_FragColor = mix( c2, c1, fr);
	}
	
}

