uniform float time;
uniform float zoom;
uniform int complexity;
uniform vec2 resolution;

//Hash Function
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

//Fractional Brownian Motion
float fbm(vec2 c, int cplx) {
    float f = 0.0;
    float w = 1.0;
    for (int i = 0; i < cplx; i++) {
        f += w * noise2f(c);
        c *= 2.0;
        w *= 0.5;
    }
    return f;
}

//Water Pattern
float pattern(vec2 p, int cplx, out vec2 q, out vec2 r){
    q.x = fbm( p + 0.00 * time, cplx);
    q.y = fbm( p + vec2(1.0), cplx);
    r.x = fbm( p + 1.0 * q + vec2(1.7, 9.2) + 0.15 * time, cplx );
    r.y = fbm( p + 1.0 * q + vec2(8.3, 2.8) + 0.126 * time, cplx);
    return fbm(p + 1.0 * r + 0.0 * time, cplx);
}

// -----------------------------------------------

void main( void )
{
    int cplx = int(clamp(float(complexity), 1., 5.));
    vec2 c = zoom * 10.0 * ((gl_FragCoord.xy -  0.5 * resolution.xy) / resolution.xy);
    vec2 q, r;
    float f = pattern(c * 0.01, cplx, q, r);
    
    gl_FragColor.rgb = vec3( f );
    gl_FragColor.a = gl_Color.a;
}