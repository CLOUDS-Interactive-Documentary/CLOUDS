#version 120
#define PI 3.14159265359
#define E 2.71828
#define LN_18 2.890371757896165

uniform float time;
uniform bool levelSet;
uniform sampler2DRect tex;
uniform sampler2DRect grunge;

/*
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
*/

float bump(float t, float center, float width){
    float f = (t - center) / width;
    return 1. - clamp(f * f, 0., 1.);
}

float heightMap(vec2 co){
    float bumps = 0;
    co *= 0.2;
    float iters = 3.;
    float t = time * .01;
    for (float i = 1.; i <= iters; i += 1.) {
        //TODO: optimize this.
        bumps += (.5 + dot(sin(co),vec2(.25))) * log(1 + i);
        co *= .07;
//        co += sin(co.yx + vec2(0.,t + PI/2.));
        co += co.yx * (1 - cos(t));
    }
    bumps /= (iters * log(iters));
    return (-.5 + pow(bumps, 18.)) * PI;
}

vec4 getLevelSet(vec4 fg){
    float a = PI * (.5 + log(.25 + fg.b) * 6.);
    float b = heightMap(gl_TexCoord[0].xy) * 18.;
    float g = fg.g + .1;
    g *= g * g;
    float levl = mix (a + b, max(a, b), .5);
    float set = .5 * (1. + sin(levl)) + g;
	return vec4(set, set, set, 1.);
}

void main(){
    vec4 color;
    vec4 fg = texture2DRect(tex, gl_TexCoord[0].xy);
    
    if (levelSet) {
            color = getLevelSet(fg);
    } else {
        vec4 bg = texture2DRect(grunge, gl_FragCoord.xy);
        //Check if you're on an area of change
        float b = 1. - fwidth(fg.b);
        
        //see if you're in the right range to be a border
        b *= fg.b * fg.b;
        float innerCell = clamp(bump(b, .8, .4), 0., 1.) * (1. - (.2 + .2 * sin(gl_FragCoord.x + gl_FragCoord.y)));
        float shell = clamp(bump(b, .3, .2), 0., .95);
        vec4 kernel = clamp(fg.g * 1.5 * vec4(.6, .7, .6, 1.), 0., 1.);
        vec4 envelope = pow(shell, 1.5) * vec4(1.);// * mix(1.,rand(gl_FragCoord.xy * 0.01), 0.15 );
        color = envelope + kernel + (innerCell - fg.g); //* vec4(0.5,0.,0.,0.3);
    }
    gl_FragColor = color;
}

