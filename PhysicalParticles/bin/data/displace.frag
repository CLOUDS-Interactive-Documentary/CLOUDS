#version 120

varying vec2 mapCoordinate;

float rand(vec2 co){
    return (sin((co.x + co.y)/20.0) + 1.0) / 3.0;
}

void main() {
    vec2 st = gl_TexCoord[0].st;
    gl_FragColor = gl_Color;
	//gl_FragColor = vec2(.1,.1);

   // gl_FragColor.rgb *= rand(mapCoordinate);
}