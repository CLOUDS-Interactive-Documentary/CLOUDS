uniform sampler2DRect map;
uniform vec2 mapDim;
uniform float polarAlphaExpo;
uniform float polarAlphaExpoScale;

varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

void main(void)
{
	vec3 normal = normalize(norm);
	float fr = pow( abs(dot( ePos, normal )), 1.);

	gl_FragColor = texture2DRect( map, uv * mapDim);
//	gl_FragColor.xyz += fr;
	gl_FragColor.w *= fr;
	gl_FragColor.xyz += pow( fr, 12.);
	gl_FragColor.w *= min(1., 1. - pow( abs(uv.y * 2. - 1.)*1.01, 10. ));
	
//	gl_FragColor = vec4( normal*.5 + .5, 1.);
}

