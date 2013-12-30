#version 120
//#extension GL_ARB_shader_texture_lod: enable
//#extension GL_ARB_texture_rectangle: enable
#define PI 3.14159265359
#define E 2.71828

uniform float time;
uniform bool levelSet;
uniform sampler2DRect tex;
uniform sampler2DRect grunge;
uniform vec2 resolution;
uniform vec2 imgRes;
uniform float translucenseDish;
uniform float translucenseCell;
uniform vec4 kernelColor_high;
uniform vec4 kernelColor_low;
uniform float kernel_maxValue;
uniform vec3 lightDirection;
uniform vec3 lightColor;


float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
/*
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

vec4 premult(in vec4 source){
    return vec4(source.rgb * source.a, source.a);
}

vec4 over(vec4 a, vec4 b){
    //a over b
    return clamp(a + b * (1. - a.a), 0., 1.);
}

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
        bumps += (.5 + dot(sin(co),vec2(.25)))
                * log(1 + i);
        co *= .07;
        co += sin(co.yx + vec2(0.,t*10. + PI/2.));
//        co += co.yx * (1 - cos(t));
    }
    bumps /= (iters * log(iters));
    return (-.5 + pow(bumps, 18.)) * PI;
}

//TODO: YOU WROTE THIS MOTHERFUCKER

float getLightIntensity(float elevation, vec2 light){
    //TODO: SMOOTH THIS
    float dx = dFdx(elevation);
    float dy = dFdy(elevation);
    //TODO: use 3d
    float intensity = clamp(length(light),0.001, 1.); //arbitrary epsilon
    return mix(1., dot(normalize(vec2(dx,dy)),normalize(light)), intensity);
}




vec4 getLevelSet(vec4 fg){
    float a = PI * (.5 + log(.25 + fg.b) * 6.);
    float b = heightMap(gl_TexCoord[0].xy) * 18.;
    float g = fg.g + .1;
    g *= g * g;
    float levl = mix (a + b, max(a, b), .5);
    float set = (.5 * (1. + sin(levl)) + g);
    float light = getLightIntensity(levl, lightDirection.xy);
	return vec4(set
                * mix(1, light, 0.3)
                * mix(vec3(1.),lightColor, light),
                1.);
}



vec4 getMicroscope(vec4 fg){
    //Check if you're on an area of change
    float b = 1. - fwidth(fg.b);
    
    //see if you're in the right range to be a border
    b *= fg.b * fg.b;
    float innerCellAlpha = clamp(bump(b, .8, .8), 0., translucenseCell);
//    innerCellAlpha *= (1. - (.2 + .2 * sin(gl_FragCoord.x + gl_FragCoord.y)));
    float shellAlpha = clamp(bump(b, .3, .2), 0., .95);
    vec4 kernel = clamp(fg.g/kernel_maxValue, 0., 1.)
//                * vec4(vec3(mix(1.,getLightIntensity(fg.g/kernel_maxValue, vec3(1)), 0.3)),1.)
                * mix(kernelColor_low, kernelColor_high, fg.g/kernel_maxValue);
    vec4 shell = vec4(1.,1.,1.,pow(shellAlpha, 1.5));
    
    vec2 normalizedCoords = gl_TexCoord[0].xy * imgRes / resolution;
    vec4 bg = texture2DRect(grunge, normalizedCoords * .5  + vec2(dFdx(fg.b),dFdy(fg.b)) * 100.0+ imgRes * .25 ); //enlarged
    
    //COMPOSITING STAGE
    vec4 ret = over(premult(shell), premult(kernel)); //top
    ret = over(premult(ret), premult(vec4(bg.rgb, bg.a*innerCellAlpha)));
    return ret;
}

void main(){
    vec4 color;
    vec4 fg = texture2DRect(tex, gl_TexCoord[0].xy);
    fg.g = sqrt(fg.g);
    if (levelSet) {
            color = getLevelSet(fg);
    } else {
        
        //This is happening because I can only use sampler2dRect
        vec2 normalizedCoords = gl_TexCoord[0].xy * imgRes / resolution;
        vec4 bg = texture2DRect(grunge, normalizedCoords);
        vec4 bg_cu = texture2DRect(grunge, normalizedCoords * .5 + imgRes * .25 ); //enlarged
        vec4 cells = getMicroscope(fg);
        bg.a *= translucenseDish;
        color = over(premult(cells), premult(bg));
    }
    
    gl_FragColor = color;
}

