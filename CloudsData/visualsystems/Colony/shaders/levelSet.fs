#version 120
//#extension GL_ARB_shader_texture_lod: enable
//#extension GL_ARB_texture_rectangle: enable
#define PI 3.14159265359

uniform sampler2DRect tex;
uniform sampler2DRect grunge;

uniform float time;

uniform bool levelSet;
uniform bool levelSetBg;

uniform vec2 resolution;
uniform vec2 imgRes;

uniform float translucenseDish;
uniform float translucenseCell;

uniform vec4 kernelColor_high;
uniform vec4 kernelColor_low;
uniform float kernel_maxValue;

uniform vec3 lightDirection;
uniform vec3 lightColor;

uniform float stippleScale;
uniform vec4 stippleColor;

//float rand(vec2 co){
//    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
//}

mat3 kernel = mat3(0.0625, 0.125,  0.0625,
                   0.125,  0.250,  0.125,
                   0.0625, 0.125,  0.0625);

vec4 convolution(in sampler2DRect screen,in vec2 coord){
    vec4 t = vec4(0.);
    for (float i = -1. ; i < 2.; i += 1.){
        for (float j = -1. ; j < 2.; j += 1.){
            t += texture2DRect(screen, coord+vec2(i, j)) * kernel[int(1. + i)][int(1. + j)];
        }
    }
    return t;
}

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
    return pow(bumps, 18.);
}


float getLightIntensity(float elevation, vec2 light){
    vec2 normal = vec2(dFdx(elevation),dFdy(elevation));
    float reflection = dot(normalize(normal), normalize(light));
    float amplitude = clamp(length(light) * length(normal),0.001, 1.);
    float amplitude_smoothed = smoothstep(0., 0.1, amplitude);
    return mix(0., reflection, amplitude_smoothed);
}




vec4 getLevelSet(vec4 fg){
    float a = PI * (.5 + log(.25 + fg.b) * 6.);
    float b = (-.5 + (levelSetBg ? heightMap(gl_TexCoord[0].xy) : 0. )) * 18. * PI;
    float g = fg.g + .1;
    g *= g * g;
    float levl = mix (a + b, max(a, b), .5);
    float set = (.5 * (1. + sin(levl)) + g/2.);
    float light = clamp(getLightIntensity(levl, lightDirection.xy),0.1,1);
	return vec4(set
                * mix(1, light, 0.5)
                * mix(vec3(1.),lightColor, smoothstep(0.2,0.7,light))
                + vec3(g),
                1.);
}



vec4 getMicroscope(vec4 fg){
    //Check if you're on an area of change
    float b = 1. - fwidth(fg.b);
    
    //see if you're in the right range to be a border
    b *= fg.b * fg.b;
    float innerCellAlpha = bump(b, .8, .8);
    float stippleAlpha = .5 * (1. + sin((gl_FragCoord.x + gl_FragCoord.y) * stippleScale)) * innerCellAlpha;
    innerCellAlpha = clamp(innerCellAlpha, 0., translucenseCell);
    vec4 stipple = stippleColor * stippleAlpha;
    float shellAlpha = clamp(bump(b, .3, .2), 0., .90);
    vec4 kernel = clamp(fg.g/kernel_maxValue, 0., 1.)
                * mix(kernelColor_low, kernelColor_high, fg.g/kernel_maxValue);
    
    vec2 normalizedCoords = gl_TexCoord[0].xy * imgRes / resolution;
    float distortion = fg.b - .25 * fg.g +shellAlpha;
    vec4 bg = texture2DRect(grunge, normalizedCoords * .5
                            + vec2(dFdx(distortion),dFdy(distortion)) * 150.0
                            + imgRes * .25 ); //enlarged
    vec4 shell = vec4(mix(vec3(1.),bg.rgb ,0.6),pow(shellAlpha, 1.5));
    
    //COMPOSITING STAGE
    vec4 ret = over(premult(shell), premult(kernel)); //top
    ret = over(ret, premult(stipple)); //no need to premult?
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
        vec4 cells = getMicroscope(fg);
        bg.a *= translucenseDish;
        color = over(premult(cells), premult(bg));
    }
    
    gl_FragColor = color;
}

