uniform sampler2D tex;
varying float size;

float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

//Hash-Based Noise Func
float noise2f( in vec2 p ){
    vec2 ip = vec2(floor(p));
    vec2 u = fract(p);
    u = u * u * (3.0 - 2.0 * u);
    
    float res = mix(
                    mix(rand(ip), rand(ip + vec2(1.0,0.0)), u.x),
                    mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x),
                    u.y);
    return res * res;
}


void main (void) {
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
    c.g += 0.3 * clamp(pow(c.g + 1., size * 0.05) - 1., 0., 1.) * (0.9 + 0.2 * rand(0.0001 * size * gl_TexCoord[0].xy));
	gl_FragColor = c * 0.8; //*gl_Color
    
}
