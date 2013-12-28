#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec4 c1;
uniform vec4 c2;
uniform float shininess;

varying vec4 color;
varying vec3 norm;
varying vec3 ePos;
varying vec2 uv;

const float LOG2 = 1.442695;

void main(void)
{
	vec3 normal = normalize(norm);
	
	float fr = abs(dot( ePos, normal ));
	
	float spec = pow( fr, shininess );
	
	float z = gl_FragCoord.z / gl_FragCoord.w;
	
	gl_FragColor = mix( c1, c2, fr ) * color;// + vec4(vec3(spec), 0.);
}

