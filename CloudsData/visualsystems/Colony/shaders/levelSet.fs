#version 120
#define PI 3.14159265359

uniform sampler2DRect tex;

mat3 kernel = mat3(	0.0625,	0.125,	0.0625,
                   0.125,	0.250,	0.125,
                   0.0625,	0.125,	0.0625);

vec4 convolution(in vec2 coord, in float range){
	vec4 t = vec4(0.);
	for (float i = -1. ; i < 2.; i += 1.){
		for (float j = -1. ; j < 2.; j += 1.){
			t += texture2DRect(tex, coord + range * vec2(i, j)) * kernel[int(1. + i)][int(1. + j)];
		}
	}
	return t;
}

void main(){
//    vec4 samp = convolution(gl_TexCoord[0].xy, 1.0);
//    float levl = 0.5 * (1. + sin(-PI * 0.5 + length(samp.xyz) * 4. * PI));
//	gl_FragColor = vec4(vec3(levl),1.);
    
    vec4 samp = texture2DRect(tex, gl_TexCoord[0].xy);
    //Check if you're on an area of change
    float b = fwidth(samp.b);
    b = smoothstep(0.005, 0.2, b);
    
//    float b = pow(1. - fwidth(samp.r) , 40.);
    
    gl_FragColor = vec4(vec3(b), 1.);
//    gl_FragColor = samp;
}
