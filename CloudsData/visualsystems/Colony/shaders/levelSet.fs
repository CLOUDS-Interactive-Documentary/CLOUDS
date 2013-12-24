#version 120
#define PI 3.14159265359
#define E 2.71828

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

vec4 getLevelSet(){
    vec4 samp = convolution(gl_TexCoord[0].xy, 1.0);
    float levl = 0.5 * (1. + sin(-PI * 0.5 + length(samp.xyz) * 4. * PI));
	return vec4(vec3(levl),1.);
}

float bump(float t, float center, float width){
    float f = pow((t - center), 2);
    return 1. - clamp(f / (width * width), 0., 1.);
}

void main(){
    vec4 samp = texture2DRect(tex, gl_TexCoord[0].xy);
    
    //Check if you're on an area of change
    float b = 1. - fwidth(samp.b);
    
    //see if you're in the right range to be a border
    
    b *= samp.b * samp.b;
    b =  bump(b, .3, 0.2) - samp.g;
    b =  clamp(b, 0., 1.);
    b += samp.g;
    b =  clamp(b, 0., 1.);
    gl_FragColor = vec4(b, b, b, 1.);
//    gl_FragColor = samp;
}
