#version 120

varying float selected;

float rand(vec2 co){
    return (sin((co.x + co.y)/20.0) + 1.0) / 3.0;
}

void main() {
	if(selected > 1.0){
		discard;
	}
//    vec2 st = gl_TexCoord[0].st;
    gl_FragColor = gl_Color;
}