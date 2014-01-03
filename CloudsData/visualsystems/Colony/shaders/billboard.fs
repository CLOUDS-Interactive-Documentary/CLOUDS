#version 120
#define EPSILON 0.01;
uniform sampler2D tex;
uniform float kernel_maxValue;
varying float size;
uniform vec3 light;


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

float getLight(){
    vec2 direction = gl_TexCoord[0].xy;// - vec2(.5);
    float l = length(direction);
    //TODO: Decide about brightness of light being emitted back from no lighitng
    float reflection = abs(dot(normalize(direction), normalize(light.xy)));
    //Min value defined here
    return mix(0.25, reflection, smoothstep(0,0.6,l));
}


void main (void) {
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
  // c.r *= (noise2f(0.01 * size * gl_TexCoord[0].xy));
    c.r = getLight();
    c.a = sqrt(c.b);
	gl_FragColor = c * kernel_maxValue;
    
}
