#version 120
#define PI 3.14159265359
#define E 2.71828

uniform sampler2DRect tex;
uniform sampler2DRect grunge;


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


float heightMap(vec2 co){
    float bumps = 0;
    co *= 0.2;
    int iters = 3;
    for (int i = 1; i <= iters; ++i) {
        bumps += .25 * (2. + sin(co.x) + sin(co.y)) * log(1 + i); //normalized
        co *= 0.07;
        co += sin(co.y) + cos(co.x);
    }
    float i = float(iters);
    bumps /= (i * log(i));
    return (-.5 + pow(bumps, 15.)) * PI;
}

vec4 getLevelSet(vec4 fg){
    float levl = max(PI * (0.5 + log(0.25 + fg.b) * 6.), heightMap(gl_FragCoord.xy) * 18.);
    float set = 0.5 * (1. + sin(levl));
    set += pow(fg.g + 0.1, 3.);
	return vec4(vec3(set), 1.);
}

float bump(float t, float center, float width){
    float f = pow((t - center), 2);
    return 1. - clamp(f / (width * width), 0., 1.);
}

void main(){
    vec4 bg = texture2DRect(grunge, gl_FragCoord.xy);
    vec4 fg = texture2DRect(tex, gl_TexCoord[0].xy);
    
    //Check if you're on an area of change
    float b = 1. - fwidth(fg.b);
    
    //see if you're in the right range to be a border
    b *= fg.b * fg.b;
    float innerCell = clamp(bump(b, .8, 0.4),0.,1.) * (1. -(0.2 + 0.2 * sin(gl_FragCoord.x + gl_FragCoord.y)) );
    float shell = clamp(bump(b, .3, 0.2), 0.,0.95);
    vec4 kernel = clamp(fg.g * 1.5 * vec4(0.6,0.7,0.6,1.),0.,1.);
    vec4 envelope = pow(shell,1.5) * vec4(1.,1.,1.,1.);// * mix(1.,rand(gl_FragCoord.xy * 0.01), 0.15 );
    vec4 color = envelope + kernel + (innerCell-fg.g); //* vec4(0.5,0.,0.,0.3);
    color = getLevelSet(fg);
    gl_FragColor = color;
}

