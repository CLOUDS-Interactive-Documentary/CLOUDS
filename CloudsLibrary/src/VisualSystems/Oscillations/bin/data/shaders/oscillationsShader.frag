 #version 110
 // #extension GL_ARB_texture_rectangle : enable

 uniform sampler2DRect screen;
 uniform vec2 resolution;

mat3 kernel = mat3(0.0625,0.125,0.0625,0.125,0.25,0.125,0.0625,0.125,0.0625);

vec4 convolution(in vec2 coord){
	vec4 t = vec4(0.);
	for (float i = -1. ; i < 2.; i+=1.){
		for (float j = -1. ; j < 2.; j+=1.){
			t += texture2DRect(screen, coord+vec2(i,j))*kernel[int(1.+i)][int(1.+j)];
		}	
	}
	return t;
}

void main(){
	vec2 coord = gl_TexCoord[0].st;

	float dist = distance(gl_TexCoord[0].st,(resolution/2.));
	dist /= (distance(resolution/2.,vec2(0.)));
	float distFactor = pow(dist, 2.0)*max(resolution.x,resolution.y)*0.33;
	
	vec2 offset = normalize(resolution/2. - coord);

	vec4 t = vec4(0.);
	t.x = convolution(coord + offset*distFactor*0.005).x;
	t.y = convolution(coord + offset*distFactor*0.045).y;
	t.z = convolution(coord + offset*distFactor*0.09).z;

	t.w=1.;
	

	gl_FragColor = t;
	
	
}
